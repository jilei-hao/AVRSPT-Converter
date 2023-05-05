#include <iostream>
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkQuadricDecimation.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkTriangleFilter.h>
#include <vtkPointData.h>

#include "common_tools.h"

int main (int argc, char *argv[])
{
  if (argc != 4)
  {
    std::cerr << "Usage: QuadricDecimate input.vtk reduction out.vtk" << std::endl;
    return EXIT_FAILURE;
  }

  std::string fnIn = argv[1];
  double reduction = std::stod(argv[2]);
  std::string fnOut = argv[3];

  vtkNew<vtkPolyDataReader> reader;
  reader->SetFileName(fnIn.c_str());
  reader->Update();

  vtkSmartPointer<vtkPolyData> polyTail = reader->GetOutput();
  std::cout << "Input #Points: " << polyTail->GetNumberOfPoints() << std::endl;
  std::cout << "Input #Triangles: " << polyTail->GetNumberOfPolys() << std::endl;
  std::cout << "Reduction: " << reduction << std::endl;

  vtkNew<vtkTriangleFilter> fltTriangle;
  fltTriangle->SetInputData(reader->GetOutput());
  fltTriangle->Update();
  polyTail = fltTriangle->GetOutput();

  std::cout << "PostTriangle: #Triangles: " << polyTail->GetNumberOfPolys() << std::endl;
  std::cout << "PostTriangle: #PointDataArrays: " << polyTail->GetPointData()->GetNumberOfArrays() << std::endl;

  vtkNew<vtkQuadricDecimation> fltQDecimate;
  fltQDecimate->SetInputData(polyTail);
  fltQDecimate->SetTargetReduction(reduction);
  fltQDecimate->SetVolumePreservation(true);
  fltQDecimate->SetMapPointData(true);
  fltQDecimate->Update();
  polyTail = fltQDecimate->GetOutput();

  std::cout << "PostDecimation: #PointDataArrays: " << polyTail->GetPointData()->GetNumberOfArrays() << std::endl;

  vtkNew<vtkPolyDataWriter> writer;
  writer->SetInputData(polyTail);
  writer->SetFileName(fnOut.c_str());
  writer->Write();

  return EXIT_SUCCESS;
}