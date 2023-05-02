#include <iostream>
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkWindowedSincPolyDataFilter.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>

int main (int argc, char *argv[])
{
  if (argc != 6)
  {
    std::cerr << "Usage: TaubinSmooth input.vtk iteration passband "
                 "feature_angle out.vtk" << std::endl;
    return EXIT_FAILURE;
  }

  std::string fnIn = argv[1];
  int iter = std::stoi(argv[2]);
  double passband = std::stod(argv[3]);
  double featureAngle = std::stod(argv[4]);
  std::string fnOut = argv[5];

  vtkNew<vtkPolyDataReader> reader;

  reader->SetFileName(fnIn.c_str());
  reader->Update();

  vtkNew<vtkWindowedSincPolyDataFilter> fltTaubin;
  fltTaubin->SetInputData(reader->GetOutput());
  fltTaubin->SetNumberOfIterations(iter);
  fltTaubin->SetPassBand(passband);

  if (featureAngle > 0)
  {
    fltTaubin->SetFeatureEdgeSmoothing(true);
    fltTaubin->SetFeatureAngle(featureAngle);
  }

  fltTaubin->Update();

  vtkNew<vtkPolyDataWriter> writer;
  writer->SetInputData(fltTaubin->GetOutput());
  writer->SetFileName(fnOut.c_str());
  writer->Write();

  return EXIT_SUCCESS;
}