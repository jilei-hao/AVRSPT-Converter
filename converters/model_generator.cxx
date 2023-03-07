#include <iostream>
#include <string>

#include <vtkNIFTIImageReader.h>
#include <vtkNew.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>

int main (int argc, char *argv[])
{
  std::cout << "======================================\n";
  std::cout << "-- AVRSPT Model Generator\n";
  std::cout << "======================================\n";

  if (argc < 3)
  {
    std::cout << "usage: ModelGen input.nii.gz output vtp" << std::endl;
    return EXIT_FAILURE;
  }

  std::string fnIn = argv[1];
  std::string fnOut = argv[2];

  std::cout << "-- Input File Name: " << fnIn << std::endl;
  std::cout << "-- Output File Name: " << fnOut << std::endl;

  // Read Image ===================

  vtkNew<vtkNIFTIImageReader> reader;
  reader->SetFileName(fnIn.c_str());
  reader->Update();

  vtkSmartPointer<vtkImageData> img_tail = reader->GetOutput();
  
  std::cout << "-- Input Image: " << std::endl;
  img_tail->Print(std::cout);

  double scalarRange[2];
  img_tail->GetScalarRange(scalarRange);
  std::cout << "-- Scalar range: [" << scalarRange[0] << ", " << scalarRange[1] << "]" << std::endl;


  // Labelwise Processing ===================

  for (int i = scalarRange[0]; i <= scalarRange[1]; ++i)
  {
    std::cout << "---- Processing Label " << i << std::endl;

    

  }





  return EXIT_SUCCESS;
}