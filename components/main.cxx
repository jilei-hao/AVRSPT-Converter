#include <iostream>
#include <vtkNIFTIImageReader.h>
#include <vtkNIFTIImageWriter.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkImageData.h>
#include <vtkSmartPointer.h>
#include <vtkMarchingCubes.h>
#include <vtkPolyDataWriter.h>
#include <vtkNew.h>

int main (int argc, char *argv[])
{
  std::string fnImageIn = "/Users/jileihao/data/avrspt/smoothing/root.nii.gz";

  vtkSmartPointer<vtkImageData> imageTail;
  vtkNew<vtkNIFTIImageReader> reader;
  reader->SetFileName(fnImageIn.c_str());
  reader->Update();
  imageTail = reader->GetOutput();

  vtkNew<vtkImageGaussianSmooth> fltGaussian;
  fltGaussian->SetInputData(imageTail);
  fltGaussian->SetStandardDeviation(2, 2, 2);
  fltGaussian->SetRadiusFactors(1, 1, 1);
  fltGaussian->Update();

  imageTail = fltGaussian->GetOutput();

  vtkNew<vtkNIFTIImageWriter> writer;
  writer->SetFileName("/Users/jileihao/data/avrspt/smoothing/root_vtksm2.nii.gz");
  writer->SetInputData(imageTail);
  writer->Write();

  vtkNew<vtkMarchingCubes> fltMC;
  fltMC->SetInputData(imageTail);
  fltMC->ComputeScalarsOff();
  fltMC->ComputeGradientsOff();
  fltMC->SetNumberOfContours(1);
  fltMC->SetValue(0, 0.0f);
  fltMC->Update();

  vtkSmartPointer<vtkPolyData> polyTail;
  vtkNew<vtkPolyDataWriter> polyWriter;
  polyWriter->SetInputData(fltMC->GetOutput());
  polyWriter->SetFileName("/Users/jileihao/data/avrspt/smoothing/root_vtksm2.vtk");
  polyWriter->Write();

  return EXIT_SUCCESS;
}