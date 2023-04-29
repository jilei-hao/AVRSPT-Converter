#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>

#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkDecimatePro.h>
#include <vtkXMLImageDataWriter.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkDiscreteFlyingEdges3D.h>
#include <itkImageToVTKImageFilter.h>
#include <itkImageFileWriter.h>
#include <itkTileImageFilter.h>

#include <PropagationAPI.h>
#include <PropagationIO.h>
#include <PropagationTools.h>
#include <PropagationParameters.h>
#include <GreedyParameters.h>
#include <PropagationCommon.h>
#include <GreedyAPI.h>

using namespace propagation;

typedef double TReal;
PROPAGATION_DATA_TYPEDEFS
typedef PropagationAPI<TReal> PropagationAPIType;
typedef PropagationInput<TReal> PropagationInputType;
typedef std::shared_ptr<PropagationInputType> PropaInputPointer;
typedef PropagationOutput<TReal> PropagationOutputType;
typedef std::shared_ptr<PropagationOutputType> PropaOutputPointer;
typedef PropagationInputBuilder<TReal> PropaInputBuilderType;
typedef PropagationTools<TReal> PropaTools;

char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
  char ** itr = std::find(begin, end, option);
  if (itr != end && ++itr != end)
  {
  return *itr;
  }
  return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
  return std::find(begin, end, option) != end;
}

inline std::string ssprintf(const char *format, ...)
{
  if(format && strlen(format))
    {
    char buffer[4096];
    va_list args;
    va_start (args, format);
    vsnprintf (buffer, 4096, format, args);
    va_end (args);
    return std::string(buffer);
    }
  else
    return std::string();
}

void usage () 
{
  std::cout << "Usage: CaseGen img4d outdir -s [seg-propagation-configs] [case-configs]\n";
  std::cout << "-- img4d: the base 4d grey image for all the processing\n";
  std::cout << "-- outdir: directory to store all the output files\n";
  std::cout << "-- seg-propagation-configs: -s seg3d tp_ref tp_target_list\n";
  std::cout << "   tp_target_list: comma separated list of the target time points\n";
  std::cout << "   use multiple -s to specify multiple segmentation configs\n";
  std::cout << "-- case-configs: \n";
  std::cout << "   -dc [decimation_percentage] (0: no decimation, 99: decimate 99\% polygons)\n";
  std::cout << "   -rs [resampling_percentage] (100: no reampling, 50: 50\% resampling)\n";
  std::cout << std::endl;
  std::cout << "Example: Following command will generate result by propagating seg02.nii.gz ";
  std::cout << "from time point 2 to time points 1,3,4,5,6, and seg09.nii.gz from timepoint 9 ";
  std::cout << "to time points 7,8,10,11,12,13,14,15,16. The image result will be resampled to 40% ";
  std::cout << "to the original resolution; The mesh result will be decimated by 50%. ";
  std::cout << "All the results will be stored at folder named output_dir.\n";
  std::cout << std::endl;
  std::cout << "CaseGen input/img4d.nii.gz output_dir \\\n";
  std::cout << "  -s seg02.nii.gz 2 1,3,4,5,6\n";
  std::cout << "  -s seg09.nii.gz 9 7,8,10,11,12,13,14,15,16\n";
  std::cout << "  -dc 50 -rs 40\n";
}

struct SegConfig
{
  std::string fnSeg;
  unsigned int tpr;
  std::vector<unsigned int> tpt; 
};

struct TPData
{
  TImage3D::Pointer img;
  TLabelImage3D::Pointer seg;
  TPropagationMeshPointer mesh;
  TPropagationMeshPointer mesh_dc;
};

TPropagationMeshPointer ProcessMesh(TPropagationMeshPointer meshIn)
{
  vtkNew<vtkDecimatePro> flt_decimate;
  flt_decimate->SetInputData(meshIn);
  flt_decimate->SetTargetReduction(70);
  flt_decimate->PreserveTopologyOn();
  flt_decimate->Update();
  return flt_decimate->GetOutput();
}

template <class TImage4D, class TImage3D>
typename TImage4D::Pointer
Create4DImageFromSeries(std::vector<typename TImage3D::Pointer> seriesIn)
{
  TImage4D *ret = nullptr;

  auto fltTile = itk::TileImageFilter<TImage3D, TImage4D>::New();

  std::cout << "[Create4DImageFromSeries] " << std::endl;
  for (auto cit = seriesIn.cbegin(); cit != seriesIn.cend(); ++cit)
  {
    auto idx = cit - seriesIn.cbegin();
    std::cout << "-- adding image: " << idx << std::endl;
    itk::FixedArray<unsigned int, 4u> layout;
    layout[0] = 1;
    layout[1] = 1;
    layout[2] = 1;
    layout[3] = 0;
    fltTile->SetLayout(layout);
    fltTile->SetInput(idx, *cit);
  }

  fltTile->SetDefaultPixelValue(0);
  fltTile->Update();

  return fltTile->GetOutput();
}

vnl_matrix_fixed<double, 4, 4>
ConstructNiftiSform(vnl_matrix<double> m_dir, vnl_vector<double> v_origin, 
	vnl_vector<double> v_spacing)
{
  // Set the NIFTI/RAS transform
  vnl_matrix<double> m_ras_matrix;
  vnl_diag_matrix<double> m_scale, m_lps_to_ras;
  vnl_vector<double> v_ras_offset;

  // Compute the matrix
  m_scale.set(v_spacing);
  m_lps_to_ras.set(vnl_vector<double>(3, 1.0));
  m_lps_to_ras[0] = -1;
  m_lps_to_ras[1] = -1;
  m_ras_matrix = m_lps_to_ras * m_dir * m_scale;

  // Compute the vector
  v_ras_offset = m_lps_to_ras * v_origin;

  // Create the larger matrix
  vnl_vector<double> vcol(4, 1.0);
  vcol.update(v_ras_offset);

  vnl_matrix_fixed<double,4,4> m_sform;
  m_sform.set_identity();
  m_sform.update(m_ras_matrix);
  m_sform.set_column(3, vcol);
  return m_sform;
}

vnl_matrix_fixed<double, 4, 4>
ConstructVTKtoNiftiTransform(vnl_matrix<double> m_dir, vnl_vector<double> v_origin, 
	vnl_vector<double> v_spacing)
{
  vnl_matrix_fixed<double,4,4> vox2nii = ConstructNiftiSform(m_dir, v_origin, v_spacing);
  vnl_matrix_fixed<double,4,4> vtk2vox;
  vtk2vox.set_identity();
  for(size_t i = 0; i < 3; i++)
    {
    vtk2vox(i,i) = 1.0 / v_spacing[i];
    vtk2vox(i,3) = - v_origin[i] / v_spacing[i];
    }
  return vox2nii * vtk2vox;
}

TPropagationMeshPointer
GetMeshFromLabelImage(typename TLabelImage3D::Pointer img)
{
  using ConvertToVTKSeg = itk::ImageToVTKImageFilter<TLabelImage3D>;
  using TVTKImagePointer = vtkSmartPointer<vtkImageData>;
  auto fltITKToVTK = ConvertToVTKSeg::New();
  fltITKToVTK->SetInput(img);
  fltITKToVTK->Update();
  TVTKImagePointer vtkimg = fltITKToVTK->GetOutput();
  
	auto range = vtkimg->GetPointData()->GetArray(0)->GetRange();
  size_t lo = 1u; // take the first label above cut
  size_t hi = floor(range[1]); // take the last integral label below imax
  size_t n = hi - lo + 1; // number of labels in the result
  std::cout << "-- Preserving " << n << " labels from " << lo << " to " << hi << std::endl;

  vtkNew<vtkDiscreteFlyingEdges3D> fltDFE;
  fltDFE->SetInputData(vtkimg);
  fltDFE->GenerateValues(n, lo, hi);
  fltDFE->Update();
  TPropagationMeshPointer poly_tail = fltDFE->GetOutput();

  auto scalars = poly_tail->GetPointData()->GetScalars();
  auto normals = poly_tail->GetPointData()->GetNormals();

  // set NaN element to 0 to prevent reader errors
  for(int i = 0; i < normals->GetNumberOfTuples(); i++)
    for(int j = 0; j < normals->GetNumberOfComponents(); j++)
    {
      if (isnan(normals->GetComponent(i, j)))
        normals->SetComponent(i, j, 0);
    }

  scalars->SetName("Label");

  auto vtk2nii = ConstructVTKtoNiftiTransform(
    img->GetDirection().GetVnlMatrix().as_ref(), 
    img->GetOrigin().GetVnlVector(),
    img->GetSpacing().GetVnlVector()
  );

  vtkNew<vtkTransform> transform;
  transform->SetMatrix(vtk2nii.data_block());
  transform->Update();
  
  vtkNew<vtkTransformPolyDataFilter> fltTransform;
  fltTransform->SetTransform(transform);
  fltTransform->SetInputData(poly_tail);
  fltTransform->Update();
  poly_tail = fltTransform->GetOutput();

	return poly_tail;
}

int main (int argc, char *argv[])
{
  std::cout << "======================================\n";
  std::cout << "-- AVRSPT Case Generator\n";
  std::cout << "======================================\n";

  // // -- PARSE COMMAND ----------
  // if (argc < 6)
  // {
  //   usage();
  //   return EXIT_FAILURE;
  // }

  // // parse image4d and the outdir first
  // std::string fnImage4D = argv[1];
  // std::string dirOutput;
  // char ** argBegin = argv;
  // char ** argEnd = argv + argc;

  std::string fnImage4D = "/Users/jileihao/data/avrspt/tav48/img4d_tav48_LAS.nii.gz";
  std::string dirOutput = "/Users/jileihao/data/avrspt/tav48/case_output";
  std::vector<SegConfig> segConfigs;
  std::map<unsigned int, TPData> tpData;

  SegConfig scSys, scDias;
  
  scSys.fnSeg = "/Users/jileihao/data/avrspt/tav48/seg02_tav48_NY.nii.gz";
  scSys.tpr = 2;
  scSys.tpt = std::vector<unsigned int>{1, 3, 4, 5};

  scDias.fnSeg = "/Users/jileihao/data/avrspt/tav48/seg09_tav48_NY.nii.gz";
  scDias.tpr = 9;
  scDias.tpt = std::vector<unsigned int>{6, 7, 8, 10, 11, 12, 13, 14};

  segConfigs.push_back(scSys);
  segConfigs.push_back(scDias);

  unsigned int dc = 0, rs = 100;

  // Running propagation
  typedef double TReal;
  using ConvertToVTKImage = itk::ImageToVTKImageFilter<TImage3D>;
  using ConvertToVTKSeg = itk::ImageToVTKImageFilter<TLabelImage3D>;

  for (auto sc : segConfigs)
  {
    std::cout << "[CaseGen] Running Propagation for config:" << std::endl;
    std::cout << "-- fnSeg: " << sc.fnSeg << std::endl;
    std::cout << "-- reference tp: " << sc.tpr << std::endl;
    std::cout << "-- target tps";
    for (auto tp : sc.tpt)
      std::cout << " " << tp;
    std::cout << std::endl;

    // -- run propagation for each segmentation config in the list

    std::shared_ptr<PropagationAPIType> propaAPI;
    std::shared_ptr<PropaInputBuilderType> ibuilder = 
      std::make_shared<PropaInputBuilderType>();

    // -- read in 4d base image
    TImage4D::Pointer img4d = PropaTools::ReadImage<TImage4D>(fnImage4D);

    // -- read in 3d reference segmentation
    TLabelImage3D::Pointer seg3d = PropaTools::ReadImage<TLabelImage3D>(sc.fnSeg);

    // -- generate a segmentation mesh
    TPropagationMeshPointer mesh = GetMeshFromLabelImage(seg3d);
    const char *debugMeshOut = "/Users/jileihao/data/avrspt/tav48/sandbox/segMesh_02.vtp";
    vtkNew<vtkXMLPolyDataWriter> debugWriter;
    debugWriter->SetInputData(mesh);
    debugWriter->SetFileName(debugMeshOut);
    debugWriter->Write();


    TPropagationMeshPointer mesh_dc = ProcessMesh(mesh);

    const char *debugMeshdcOut = "/Users/jileihao/data/avrspt/tav48/sandbox/segMesh_dc_02.vtp";
    debugWriter->SetInputData(mesh_dc);
    debugWriter->SetFileName(debugMeshdcOut);
    debugWriter->Write();

    std::cout << "Original: " << mesh->GetNumberOfPolys() << std::endl;
    std::cout << "Decimated: " << mesh_dc->GetNumberOfPolys() << std::endl;

    std::string meshTag = "dc";

    tpData[sc.tpr].seg = seg3d; // put reference segmentation in

    ibuilder->SetImage4D(img4d);
    ibuilder->SetReferenceSegmentationIn3D(seg3d);
    ibuilder->SetReferenceTimePoint(sc.tpr);
    ibuilder->SetTargetTimePoints(sc.tpt);
    ibuilder->SetResliceMetricToLabel(0.2, false); // LABEL 0.2 vox
    ibuilder->SetRegistrationMetric(GreedyParameters::SSD);
    ibuilder->SetMultiResolutionSchedule(std::vector<int>{ 50, 50 }); // 100x100
    ibuilder->SetAffineDOF(GreedyParameters::AffineDOF::DOF_RIGID);
    ibuilder->AddExtraMeshToWarp(mesh_dc, meshTag);
    ibuilder->SetPropagationVerbosity(PropagationParameters::Verbosity::VERB_VERBOSE);
    PropaInputPointer propaInput = ibuilder->BuildPropagationInput();

    propaAPI = std::make_shared<PropagationAPIType>(propaInput);
    propaAPI->Run();

    PropaOutputPointer propaOut = propaAPI->GetOutput();
    auto outMeshes = propaOut->GetMeshSeries();
    auto outSegs = propaOut->GetSegmentation3DSeries();
    auto outExtraMesh = propaOut->GetExtraMeshSeries(meshTag);

    std::vector<unsigned int> alltp {sc.tpt};
    alltp.push_back(sc.tpr);

    for (auto tp : alltp)
    {
      TPData tpd;
      auto drs = rs * 0.01; // decimal resample rate

      // process grey image
      auto img3d = PropaTools::template 
        ExtractTimePointImage<TImage3D, TImage4D>(img4d, tp);
      tpd.img = PropaTools::template
        Resample3DImage<TImage3D>(img3d, drs, ResampleInterpolationMode::Linear);

      // process seg image
      auto seg3d = outSegs[tp];
      tpd.seg = PropaTools::template
        Resample3DImage<TLabelImage3D>(seg3d, drs, ResampleInterpolationMode::NearestNeighbor);

      tpd.mesh = outMeshes[tp];
      tpd.mesh_dc = outExtraMesh[tp];
      tpData[tp] = tpd;
    }
  }

  std::cout << "[CaseGen] Final Result: " << std::endl;

  for (auto kv : tpData)
  {
    auto tp = kv.first;
    auto data = kv.second;

    std::cout << " -- tp = " << tp 
              << " img: " << data.img.GetPointer()
              << " seg: " << data.seg.GetPointer()
              << " mesh: " << data.mesh
              << " mesh_dc: " << data.mesh_dc
              << std::endl;
    
    // write out 3D Grey Image Series
    std::ostringstream oss_img;
    oss_img << dirOutput << PropaTools::GetPathSeparator();
    oss_img << "img3d_rs%03d_%02d.vti";
    std::string fnout_img3d = ssprintf(oss_img.str().c_str(), rs, tp);

    
    auto vtkFilter = ConvertToVTKImage::New(); // convert itk image to vtk image
    vtkFilter->SetInput(data.img);
    vtkFilter->Update(); 
    vtkNew<vtkXMLImageDataWriter> vtkWriter; // write to vti format
    vtkWriter->SetInputData(vtkFilter->GetOutput());
    vtkWriter->SetFileName(fnout_img3d.c_str());
    vtkWriter->Write();
    
    // write out 3D Segmentation Image Series
    std::ostringstream oss_seg;
    oss_seg << dirOutput << PropaTools::GetPathSeparator();
    oss_seg << "seg3d_rs%03d_%02d.vti";
    std::string fnout_seg3d = ssprintf(oss_seg.str().c_str(), rs, tp);

    
    auto vtkFilterSeg = ConvertToVTKSeg::New();
    vtkFilterSeg->SetInput(data.seg);
    vtkFilterSeg->Update();
    vtkWriter->SetInputData(vtkFilterSeg->GetOutput());
    vtkWriter->SetFileName(fnout_seg3d.c_str());
    vtkWriter->Write();

    // write out 3D Mesh Series
    std::ostringstream oss_mesh;
    oss_mesh << dirOutput << PropaTools::GetPathSeparator();
    oss_mesh << "mesh_dc%02d_%02d.vtp";
    std::string fnout_mesh = ssprintf(oss_mesh.str().c_str(), dc, tp);

    vtkNew<vtkXMLPolyDataWriter> vtpWriter;
    vtpWriter->SetInputData(data.mesh_dc);
    vtpWriter->SetFileName(fnout_mesh.c_str());
    vtpWriter->Write();
    
  }

  // Debugging code
  // std::vector<TImage3D::Pointer> image3DSeries;
  // std::vector<TLabelImage3D::Pointer> seg3DSeries;

  // for (auto kv : tpData)
  // {
  //   auto tp = kv.first;
  //   auto data = kv.second;
  //   image3DSeries.push_back(data.img);
  //   seg3DSeries.push_back(data.seg);
  // }

  // TImage4D::Pointer imgTiled = 
  //   Create4DImageFromSeries<TImage4D, TImage3D>(image3DSeries);

  // TLabelImage4D::Pointer segTiled = 
  //   Create4DImageFromSeries<TLabelImage4D, TLabelImage3D>(seg3DSeries);

  // // write out 4d segmentation for troubleshooting
  // std::ostringstream oss_seg4d;
  // oss_seg4d << dirOutput << PropaTools::GetPathSeparator();
  // oss_seg4d << "seg4d.nii.gz";
  // auto itkSegWriter = itk::ImageFileWriter<TLabelImage4D>::New();
  // itkSegWriter->SetInput(segTiled);
  // itkSegWriter->SetFileName(oss_seg4d.str().c_str());
  // itkSegWriter->Write();

  // // write out 4d image for troubleshooting
  // std::ostringstream oss_img4d;
  // oss_img4d << dirOutput << PropaTools::GetPathSeparator();
  // oss_img4d << "img4d.nii.gz";
  // auto itkImageWriter = itk::ImageFileWriter<TImage4D>::New();
  // itkImageWriter->SetInput(imgTiled);
  // itkImageWriter->SetFileName(oss_img4d.str().c_str());
  // itkImageWriter->Write();

  return EXIT_SUCCESS;
}