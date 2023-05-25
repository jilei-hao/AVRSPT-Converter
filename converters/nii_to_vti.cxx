#include <iostream>
#include <vtkNIFTIImageReader.h>
#include <vtkXMLImageDataWriter.h>
#include <vtkNew.h>
#include <vtkImageData.h>

int main (int argc, char *argv[])
{
  if (argc < 3)
  {
    std::cerr << "Usage: nii2vti input.nii.gz output.vti" << std::endl;
    return EXIT_FAILURE;
  }
  
  vtkNew<vtkNIFTIImageReader> reader;
  reader->SetFileName(argv[1]);
  reader->Update();

  vtkNew<vtkXMLImageDataWriter> writer;
  writer->SetFileName(argv[2]);
  writer->SetInputData(reader->GetOutput());
  writer->Write();

  return EXIT_SUCCESS;  
}
