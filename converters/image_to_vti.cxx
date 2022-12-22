#include <iostream>
#include <vtkNIFTIImageReader.h>
#include <vtkXMLImageDataWriter.h>
#include <vtkNew.h>
#include <vtkImageData.h>

int main (int argc, char *argv[])
{
  std::cout << "-- Input Filename: " << argv[1] << std::endl;
  std::cout << "-- Output Filename: " << argv[2] << std::endl;
  vtkNew<vtkNIFTIImageReader> reader;
  reader->SetFileName(argv[1]);
  reader->Update();

  vtkNew<vtkXMLImageDataWriter> writer;
  writer->SetFileName(argv[2]);
  writer->SetInputData(reader->GetOutput());
  writer->Write();

  return EXIT_SUCCESS;  
}
