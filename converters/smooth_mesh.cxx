#include <iostream>
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>

int main (int argc, char *argv[])
{

  std::string fnIn = argv[1];
  int iter = std::stoi(argv[2]);
  double passband = std::stod(argv[3]);
  double featureAngle = std::stod(argv[4]);
  std::string fnOut = argv[4];

  vtkNew<vtkPolyDataReader> reader;

  reader->SetFileName(fnIn.c_str());
  reader->Update();

  vtkNew<vtkWindowedSincPolyDataFilter> fltTaubin;
  fltTaubin->SetInputData(reader->GetOutput());
  fltTaubin->SetNumberOfIterations(iter);
  fltTaubin->SetPassBand(passband);
  fltTaubin->SetFeatureEdgeSmoothing(true);
  fltTaubin->SetFeatureAngle(featureAngle);

  fltTaubin->Update();


  vtkNew<vtkPolyDataWriter> writer;
  writer->SetInputData(fltTaubin->GetOutput());
  writer->SetFileName(fnOut.c_str());
  writer->Write();

  return EXIT_SUCCESS;
}