#include <iostream>
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkTriangleFilter.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>

#include "common_tools.h"

int main (int argc, char *argv[])
{
  if (argc != 6)
  {
    std::cerr << "Usage: LaplacianSmooth input.vtk iteration relaxationFactor "
                 "feature_angle out.vtk" << std::endl;
    return EXIT_FAILURE;
  }

  std::string fnIn = argv[1];
  int iter = std::stoi(argv[2]);
  double relaxation = std::stod(argv[3]);
  double featureAngle = std::stod(argv[4]);
  std::string fnOut = argv[5];

  vtkNew<vtkPolyDataReader> reader;
  reader->SetFileName(fnIn.c_str());
  reader->Update();

  vtkSmartPointer<vtkPolyData> polyTail;
  polyTail = reader->GetOutput();

  vtkNew<vtkTriangleFilter> fltTriangle;
  fltTriangle->SetInputData(polyTail);
  fltTriangle->Update();
  polyTail = fltTriangle->GetOutput();

  vtkNew<vtkSmoothPolyDataFilter> fltLaplacian;
  fltLaplacian->SetInputData(polyTail);
  fltLaplacian->SetNumberOfIterations(iter);
  fltLaplacian->SetRelaxationFactor(relaxation);
  if (featureAngle > 0)
  {
    fltLaplacian->SetFeatureEdgeSmoothing(true);
    fltLaplacian->SetFeatureAngle(featureAngle);
  }


  fltLaplacian->Update();


  vtkNew<vtkPolyDataWriter> writer;
  writer->SetInputData(fltLaplacian->GetOutput());
  writer->SetFileName(fnOut.c_str());
  writer->Write();

  return EXIT_SUCCESS;
}