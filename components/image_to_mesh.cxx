#include "image_to_mesh.h"

ImageToMesh
::ImageToMesh()
{

}

ImageToMesh
::~ImageToMesh()
{

}

void
ImageToMesh
::SetImage(vtkSmartPointer<vtkImageData> image)
{
  m_ImageIn = image;
}


vtkSmartPointer<vtkPolyData>
ImageToMesh
::GetOutput()
{
  return m_MeshOut;
}