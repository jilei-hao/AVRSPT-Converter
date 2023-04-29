#ifndef __image_to_mesh_h_
#define __image_to_mesh_h_

#include <iostream>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkPolyData.h>

class ImageToMesh
{
public:
  ImageToMesh();
  ~ImageToMesh();
  ImageToMesh(const ImageToMesh &other) = delete;
  ImageToMesh & operator=(const ImageToMesh &other) = delete;

  void SetImage(vtkSmartPointer<vtkImageData> image);

  vtkSmartPointer<vtkPolyData> GetOutput();

private:
  vtkSmartPointer<vtkImageData> m_ImageIn;
  vtkSmartPointer<vtkPolyData> m_MeshOut;
};

#endif // __image_to_mesh_h_