#include <iostream>
#include <string>
#include <vtkPolyDataReader.h>
#include <vtkDoubleArray.h>
#include <vtkSmartPointer.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkPolyData.h>
#include <vtkFieldData.h>
#include <vtkNew.h>

int main (int argc, char *argv[])
{
  std::cout << "==================================================\n";
  std::cout << "============= VTK to VTP converter ===============\n";

  if (argc < 5) 
  {
      std::cout << "\n usage VTKToVTP input.vtk output.vtp key value\n";
      
      return EXIT_FAILURE;
  }
  
  std::string fnIn = argv[1];
  std::string fnOut = argv[2];
  std::string key = argv[3];
  double value = std::stod(argv[4]);

  std::cout << "-- Input: " << fnIn << std::endl;
  std::cout << "-- Output: " << fnOut << std::endl;
  std::cout << "-- key: " << key << std::endl;
  std::cout << "-- Value: " << value << std::endl;

  vtkNew<vtkPolyDataReader> reader;
  reader->SetFileName(fnIn.c_str());
  reader->Update();

  vtkSmartPointer<vtkPolyData> poly_tail;
  poly_tail = reader->GetOutput();

  vtkNew<vtkDoubleArray> arr;
  arr->SetNumberOfValues(1);
  arr->SetName(key.c_str());
  arr->SetValue(0, value);

  poly_tail->GetFieldData()->AddArray(arr);

  vtkNew<vtkXMLPolyDataWriter> writer;
  writer->SetInputData(poly_tail);
  writer->SetFileName(fnOut.c_str());
  writer->Write();

  return EXIT_SUCCESS;
}