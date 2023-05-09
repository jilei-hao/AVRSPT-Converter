#include <iostream>
#include <string>

#include <itkImageFileReader.h>

#include <vtkPolyDataReader.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkNIFTIImageReader.h>
#include <vtkImageData.h>
#include <vtkImageThreshold.h>
#include <vtkMarchingCubes.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkPolyDataWriter.h>
#include <vtkQuadricDecimation.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkTriangleFilter.h>
#include <vtkUnsignedIntArray.h>
#include <vtkPointData.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>


#include "mesh_helpers.h"
#include "itkOrientedRASImage.h"

typedef vtkSmartPointer<vtkPolyData> MeshPointer;
typedef uint16_t LabelType;
typedef itk::OrientedRASImage<LabelType, 3U> LabelImage3DType;
typedef vtkSmartPointer<vtkImageData> VTKImagePointer;

bool indexInExtent(VTKImagePointer img, int index[3])
{
  int extent[6];
  img->GetExtent(extent);
  return (index[0] >= extent[0] && index[0] <= extent[1] &&
      index[1] >= extent[2] && index[1] <= extent[3] &&
      index[2] >= extent[4] && index[2] <= extent[5]);
}

double findNearestNonBackgroundVoxelValue(VTKImagePointer img, int imgIJK[3], uint16_t r)
{
  unsigned long long minDistSq = VTK_UNSIGNED_LONG_LONG_MAX;
  double crntValue = 0;

  for (auto di = -r; di <= r; ++di)
    for (auto dj = -r; dj <= r; ++dj)
      for (auto dk = -r; dk <= r; ++dk)
      {
        if (di == 0 && dj == 0 && dk == 0)
          continue;

        int crntIJK[3] = {imgIJK[0] + di, imgIJK[1] + dj, imgIJK[2] + dk};

        if (!indexInExtent(img, crntIJK))
          continue;

        double v = img->GetScalarComponentAsDouble(crntIJK[0], crntIJK[1], crntIJK[2], 0);

        if (v == 0)
          continue;

        unsigned long long crntDistSq = 
          crntIJK[0] * crntIJK[0] + 
          crntIJK[1] * crntIJK[1] + 
          crntIJK[2] * crntIJK[2];

        if (crntDistSq == 1)
          return v; // direct return if distance is 1

        if (crntDistSq < minDistSq)
        {
          minDistSq = crntDistSq;
          crntValue = v;
        }
      }

  return crntValue;
}

int main (int argc, char *argv[])
{
  if (argc < 3)
  {
    std::cout << "Usage: TestCommon input.nii.gz output.vtk" << std::endl;
    return EXIT_FAILURE;
  }

  std::string fnImg = argv[1];
  std::string fnMeshOut = argv[2];

  vtkNew<vtkNIFTIImageReader> imgReader;
  imgReader->SetFileName(fnImg.c_str());
  imgReader->Update();
  vtkNew<vtkImageData> img;
  img->DeepCopy(imgReader->GetOutput());

  std::cout << "Image Print: " << std::endl;
  img->Print(std::cout);

  auto imgRAS = itk::ReadImage<LabelImage3DType>(fnImg);
  auto vtk2nii = MeshHelpers::getVTKToNiftiTransform(imgRAS);

  vtkNew<vtkImageThreshold> fltThreshold;
  fltThreshold->SetInputData(img);
  fltThreshold->ThresholdBetween(1, VTK_DOUBLE_MAX);
  fltThreshold->SetInValue(1);
  fltThreshold->SetOutValue(0);
  fltThreshold->Update();

  auto bimg = fltThreshold->GetOutput();

  vtkNew<vtkMarchingCubes> fltMC;
  fltMC->SetInputData(bimg);
  fltMC->SetValue(0, 1.0);
  fltMC->ComputeNormalsOn();
  fltMC->ComputeGradientsOff();
  fltMC->ComputeScalarsOff();
  fltMC->Update();

  auto mesh = fltMC->GetOutput();

  vtkNew<vtkQuadricDecimation> fltQDecimate;
  fltQDecimate->SetInputData(mesh);
  fltQDecimate->SetVolumePreservation(true);
  fltQDecimate->SetMapPointData(false);
  fltQDecimate->SetTargetReduction(0.2);
  fltQDecimate->Update();
  mesh = fltQDecimate->GetOutput();

  vtkNew<vtkWindowedSincPolyDataFilter> fltTaubin;
  fltTaubin->SetInputData(mesh);
  fltTaubin->SetNumberOfIterations(30);
  fltTaubin->SetPassBand(0.1);
  fltTaubin->Update();
  mesh = fltTaubin->GetOutput();

  vtkNew<vtkQuadricDecimation> fltQDecimate2;
  fltQDecimate2->SetInputData(mesh);
  fltQDecimate2->SetVolumePreservation(true);
  fltQDecimate2->SetMapPointData(false);
  fltQDecimate2->SetTargetReduction(0.8);
  fltQDecimate2->Update();
  mesh = fltQDecimate2->GetOutput();

  vtkNew<vtkWindowedSincPolyDataFilter> fltTaubin2;
  fltTaubin2->SetInputData(mesh);
  fltTaubin2->SetNumberOfIterations(30);
  fltTaubin2->SetPassBand(0.1);
  fltTaubin2->Update();
  mesh = fltTaubin2->GetOutput();

  vtkNew<vtkUnsignedIntArray> labelArr;
  labelArr->SetNumberOfComponents(1);
  labelArr->Allocate(mesh->GetNumberOfPoints());

  uint16_t zeroCnt = 0;
  for(vtkIdType i = 0; i < mesh->GetNumberOfPoints(); ++i)
  {
    auto pts = mesh->GetPoints();
    auto p = pts->GetPoint(i);

    int imgIJK[3];
    double pcoord[3];
    img->ComputeStructuredCoordinates(p, imgIJK, pcoord);
    double pv = img->GetScalarComponentAsDouble(imgIJK[0], imgIJK[1], imgIJK[2], 0);

    if (pv == 0)
      pv = findNearestNonBackgroundVoxelValue(img, imgIJK, 2);

    // std::cout << "mesh coord: [" << p[0] << "," << p[1] << "," << p[2] << "]"
    //           << " img coord: [" << imgIJK[0] << "," << imgIJK[1] << "," << imgIJK[2] << "]"
    //           << " img pixel: " << pv
    //           << std::endl;

    labelArr->InsertNextTuple1(pv);

    if (pv == 0)
      zeroCnt++;

  }

  labelArr->SetName("Label");
  mesh->GetPointData()->SetScalars(labelArr);
  std::cout << "zeroCnt: " << zeroCnt << std::endl;


  vtkNew<vtkTransformPolyDataFilter> fltTransform;
  fltTransform->SetInputData(mesh);
  fltTransform->SetTransform(vtk2nii);
  fltTransform->Update();
  mesh = fltTransform->GetOutput();
  
  vtkNew<vtkPolyDataWriter> vtkWriter;
  vtkWriter->SetInputData(mesh);
  vtkWriter->SetFileName(fnMeshOut.c_str());
  vtkWriter->Update();

  return EXIT_SUCCESS;
}