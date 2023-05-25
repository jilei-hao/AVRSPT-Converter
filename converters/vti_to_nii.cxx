#include <iostream>
#include <vtkSmartPointer.h>
#include <vtkNew.h>
#include <vtkImageData.h>
#include <vtkXMLImageDataReader.h>
#include <vtkNIFTIImageWriter.h>

int main (int argc, char *argv[])
{
  if (argc < 3)
  {
    std::cerr << "Usage: vti2nii input.vti output.nii.gz" << std::endl;
  }

  vtkNew<vtkXMLImageDataReader> reader;
  reader->SetFileName(argv[1]);
  reader->Update();

  vtkNew<vtkNIFTIImageWriter> writer;
  writer->SetFileName(argv[2]);
  writer->SetInputData(reader->GetOutput());
  writer->Write();

  return EXIT_SUCCESS;
}