#include <iostream>

#include <vtkXMLImageDataReader.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>

int main (int argc, char *argv[]) {
  if (argc < 2)
  {
    std::cerr << "Usage: VTIProbe image.vti" << std::endl;
    return EXIT_FAILURE;
  }

  vtkNew<vtkXMLImageDataReader> reader;
  reader->SetFileName(argv[1]);
  reader->Update();
  reader->GetOutput()->Print(std::cout);


  return EXIT_SUCCESS;
}