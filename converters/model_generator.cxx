#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <limits>

#include <itkImageFileReader.h>
#include <itkImage.h>
#include <itkImageFileWriter.h>
#include <itkIdentityTransform.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkNearestNeighborInterpolateImageFunction.h>
#include <itkResampleImageFilter.h>
#include <itkSmoothingRecursiveGaussianImageFilter.h>
#include <itkMultiThreaderBase.h>
#include <itkImageToVTKImageFilter.h>

#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkPolyData.h>
#include <vtkMarchingCubes.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkPolyDataWriter.h>

#include "mesh_helpers.h"
#include "common_tools.h"

typedef uint16_t LabelType;
typedef double RealType;
typedef itk::Image<LabelType, 3u> LabelImage3DType;
typedef vtkPolyData MeshType;
typedef vtkSmartPointer<MeshType> MeshPointer;
typedef vtkSmartPointer<vtkImageData> vtkImagePointer;

static RealType REAL_INF = std::numeric_limits<RealType>::infinity();

struct ImageStepConfig
{
  uint16_t resampleRate;
  double gaussianSigma;
  ImageStepConfig(uint16_t rs, double sigma)
    :resampleRate(rs), gaussianSigma(sigma) {};
};

struct MeshStepConfig
{
  double decimateRate;
  uint16_t smoothIteration = 20;
  double smoothPassband = 0.1;
  uint16_t smoothFeatureAngle = 0;
  MeshStepConfig(double dc, uint16_t iter, double pb, uint16_t fa)
    : decimateRate(dc), smoothIteration(iter), smoothPassband(pb), smoothFeatureAngle(fa) {};
};

struct LabelConfig
{
  std::vector<ImageStepConfig> imgConfigs;
  std::vector<MeshStepConfig> meshConfigs;
};

LabelConfig createLabelConfig(char size) // S, M, L
{
  LabelConfig ret;
  

  switch(size)
  {
    case 'S': 
    {
      ret.imgConfigs.push_back(ImageStepConfig(200, 2));
      ret.imgConfigs.push_back(ImageStepConfig(200, 2));
      ret.meshConfigs.push_back(MeshStepConfig(0.2, 20, 0.1, 0));
      ret.meshConfigs.push_back(MeshStepConfig(0.5, 20, 0.1, 0));
      ret.meshConfigs.push_back(MeshStepConfig(0.5, 20, 0.1, 0));
      break;
    }
    case 'M':
    {
      ret.imgConfigs.push_back(ImageStepConfig(200, 2));
      ret.imgConfigs.push_back(ImageStepConfig(200, 2));
      ret.meshConfigs.push_back(MeshStepConfig(0.2, 20, 0.1, 0));
      ret.meshConfigs.push_back(MeshStepConfig(0.5, 20, 0.1, 0));
      ret.meshConfigs.push_back(MeshStepConfig(0.5, 20, 0.1, 0));
      ret.meshConfigs.push_back(MeshStepConfig(0.5, 20, 0.1, 0));
      break;
    }
    case 'L':
    {
      ret.imgConfigs.push_back(ImageStepConfig(200, 2));
      ret.imgConfigs.push_back(ImageStepConfig(200, 3));
      ret.meshConfigs.push_back(MeshStepConfig(0.2, 100, 0.01, 0));
      ret.meshConfigs.push_back(MeshStepConfig(0.75, 20, 0.1, 0));
      ret.meshConfigs.push_back(MeshStepConfig(0.75, 20, 0.1, 0));
      break;
    }
  }

  return ret;
}

LabelImage3DType::Pointer
resampleImage(LabelImage3DType::Pointer img, uint16_t rate)
{
  std::cout << "-- resampling with scale: " << rate << "%" <<  std::endl;
  double ratio = (double)rate/100.0;

  typedef itk::NearestNeighborInterpolateImageFunction<LabelImage3DType> NNInterpolatorType;
  
  auto fltResample = itk::ResampleImageFilter<LabelImage3DType, LabelImage3DType>::New();
  fltResample->SetInput(img);

  auto transform = itk::IdentityTransform<double, 3>::New();
  auto interpolator = NNInterpolatorType::New();
  transform->SetIdentity();
  fltResample->SetTransform(transform);
  fltResample->SetInterpolator(interpolator);

  const LabelImage3DType::RegionType &inRegion = img->GetLargestPossibleRegion();
  const LabelImage3DType::SizeType &inSize = inRegion.GetSize();
  auto &inDirection = img->GetDirection();
  auto &inOrigin = img->GetOrigin();

  // set out spacing.
  const LabelImage3DType::SpacingType &inSpacing = img->GetSpacing();
  double outSpacing[3];
  LabelImage3DType::SizeType outSize;
  for (int i = 0; i < 3; ++i)
  {
    outSpacing[i] = inSpacing[i] / ratio;
    outSize[i] = inSize[i] * ratio;
  }

  fltResample->SetOutputOrigin(inOrigin);
  fltResample->SetSize(outSize);
  fltResample->SetOutputSpacing(outSpacing);
  fltResample->SetOutputDirection(inDirection);
  fltResample->Update();
  
  return fltResample->GetOutput();
}

template<typename TInputPixel, typename TOutputPixel>
typename itk::Image<TOutputPixel, 3u>::Pointer
thresholdImage(typename itk::Image<TInputPixel, 3u>::Pointer img, TInputPixel lo, TInputPixel hi,
  TOutputPixel in, TOutputPixel out)
{
  std::cout << "-- thresholding... lo: " << lo << "; hi: " << hi 
            << "; in: " << in << "; out: " << out << std::endl;

  // threshold
  auto fltThreshold = 
    itk::BinaryThresholdImageFilter<itk::Image<TInputPixel, 3u>, itk::Image<TOutputPixel, 3u>>::New();

  fltThreshold->SetInput(img);
  fltThreshold->SetLowerThreshold(lo);
  fltThreshold->SetUpperThreshold(hi);
  fltThreshold->SetInsideValue(in);
  fltThreshold->SetOutsideValue(out);
  fltThreshold->Update();

  return fltThreshold->GetOutput();
}

LabelImage3DType::Pointer
gaussianSmooth(LabelImage3DType::Pointer img, double voxSigma)
{
  std::cout << "-- gaussian smoothing sigma: " << voxSigma << "vox" << std::endl;
  typedef itk::Image<RealType, 3u> RealImage3DType;
  typedef itk::SmoothingRecursiveGaussianImageFilter<LabelImage3DType, RealImage3DType> SmoothingFilterType;
  auto fltGaussian = SmoothingFilterType::New();

  LabelImage3DType::SpacingType spc = img->GetSpacing();

  fltGaussian->SetInput(img);

  SmoothingFilterType::SigmaArrayType sigmaArr;
  for (int i = 0; i < 3; ++i)
  {
    sigmaArr[i] = voxSigma * spc[i];
  }

  fltGaussian->SetSigmaArray(sigmaArr);
  fltGaussian->Update();

  return thresholdImage<RealType, LabelType>(fltGaussian->GetOutput(), 0.5, REAL_INF, 1u, 0u);
}

LabelImage3DType::Pointer 
processImage(LabelImage3DType::Pointer img_ml, LabelType label, 
  std::vector<ImageStepConfig> &config, bool debug = false)
{
  std::cout << "---------------------------------------------\n";
  std::cout << "-- processing image for label: " << label << std::endl;
  std::cout << "---------------------------------------------\n";


  // threshold
  auto img_th = thresholdImage<LabelType, LabelType>(img_ml, label, label, 1u, 0u);
  auto img_thsm = gaussianSmooth(img_th, 1);

  if (debug)
  {
    auto fndebug = CommonTools::ssprintf(
      "/Users/jileihao/data/avrspt/smoothing/label%02d_thsm.nii.gz", label);
    itk::WriteImage(img_thsm, fndebug);
  }
  
  auto imgTail = img_thsm;

  int debugCnt = 0;
  for (ImageStepConfig &isc : config)
  {
    ++debugCnt;
    // upsample
    auto img_rs = resampleImage(imgTail, isc.resampleRate);
    imgTail = gaussianSmooth(img_rs, isc.gaussianSigma);

    if (debug)
    {
      auto fndebug = CommonTools::ssprintf(
      "/Users/jileihao/data/avrspt/smoothing/label%02d_iter%02d_rs.nii.gz", label, debugCnt);
      itk::WriteImage(img_rs, fndebug);
      
      fndebug = CommonTools::ssprintf(
      "/Users/jileihao/data/avrspt/smoothing/label%02d_iter%02d_rssm.nii.gz", label, debugCnt);
      itk::WriteImage(imgTail, fndebug);
    }    
  }

  return imgTail;
}

MeshPointer getMeshFromBinaryImage(LabelImage3DType::Pointer img, double threshold)
{
  std::cout << "-- getting mesh from bianry image. threshold: " << threshold << std::endl;

  auto fltITK2VTK = itk::ImageToVTKImageFilter<LabelImage3DType>::New();
  fltITK2VTK->SetInput(img);
  fltITK2VTK->Update();
  vtkImagePointer vtkImg = fltITK2VTK->GetOutput();
  auto vtk2niiTransform = MeshHelpers::getVTKToNiftiTransform(img);

  vtkNew<vtkMarchingCubes> fltMC;
  fltMC->SetInputData(vtkImg);
  fltMC->SetValue(0, threshold);
  fltMC->ComputeNormalsOn();
  fltMC->Update();

  MeshPointer polyTail = fltMC->GetOutput();

  std::cout << "---- vtk2niiTransform: "  << std::endl;
  vtk2niiTransform->Print(std::cout);

  // vtkNew<vtkTransformPolyDataFilter> fltTransform;
  // fltTransform->SetTransform(vtk2niiTransform);
  // fltTransform->SetInputData(polyTail);
  // fltTransform->Update();
  // polyTail = fltTransform->GetOutput();

  return polyTail;
}

void WriteMesh(MeshPointer mesh, std::string &fn)
{
  vtkNew<vtkPolyDataWriter> writer;
  writer->SetInputData(mesh);
  writer->SetFileName(fn.c_str());
  writer->Write();
}


int main (int argc, char *argv[])
{
  std::cout << "======================================\n";
  std::cout << "-- AVRSPT Model Generator\n";
  std::cout << "======================================\n";

  if (argc < 3)
  {
    std::cout << "usage: ModelGen input.nii.gz output.vtp" << std::endl;
    return EXIT_FAILURE;
  }

  bool debug = true;

  itk::MultiThreaderBase::SetGlobalDefaultNumberOfThreads(8);
  itk::MultiThreaderBase::SetGlobalMaximumNumberOfThreads(32);

  std::string fnImageIn = argv[1];
  std::string fnMeshOut = argv[2];

  std::cout << "-- Input File Name: " << fnImageIn << std::endl;
  std::cout << "-- Output File Name: " << fnMeshOut << std::endl;

  // read image
  auto imgIn = itk::ReadImage<LabelImage3DType>(fnImageIn);

  std::map<LabelType, LabelConfig> labelConfigs;
  labelConfigs[1] = createLabelConfig('M');
  // labelConfigs[2] = createLabelConfig('M');
  // labelConfigs[3] = createLabelConfig('M');
  // labelConfigs[4] = createLabelConfig('L');
  // labelConfigs[5] = createLabelConfig('S');
  // labelConfigs[6] = createLabelConfig('S');


  for (auto kv : labelConfigs)
  {
    // get upsampled smoothed image
    auto img_rssm = processImage(imgIn, kv.first, kv.second.imgConfigs, false);

    if (debug)
    {
      auto fndebug = CommonTools::ssprintf(
        "/Users/jileihao/data/avrspt/smoothing/label-%02d_rssm.nii.gz", kv.first);
      itk::WriteImage(img_rssm, fndebug);
    }

    // get mesh
    auto mesh = getMeshFromBinaryImage(img_rssm, 0.5);

    if (debug)
    {
      auto fnDebugMesh = CommonTools::ssprintf(
      "/Users/jileihao/data/avrspt/smoothing/label-%02d.vtk", kv.first);
      WriteMesh(mesh, fnDebugMesh);
    }
  
  }

  return EXIT_SUCCESS;
}