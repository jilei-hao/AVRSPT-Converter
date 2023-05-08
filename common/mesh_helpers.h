#ifndef __mesh_helpers_h_
#define __mesh_helpers_h_

#include <iostream>

#include <itkImage.h>
#include <vtkSmartPointer.h>
#include <vtkTransform.h>

class MeshHelpers
{
public:
  MeshHelpers();
  ~MeshHelpers();

  static vnl_matrix_fixed<double, 4, 4> ConstructNiftiSform(
    vnl_matrix<double> m_dir, vnl_vector<double> v_origin, vnl_vector<double> v_spacing);

  static vnl_matrix_fixed<double, 4, 4> ConstructVTKtoNiftiTransform(
    vnl_matrix<double> m_dir, vnl_vector<double> v_origin, vnl_vector<double> v_spacing);

  static vtkSmartPointer<vtkTransform> getVTKToNiftiTransform(itk::ImageBase<3>::Pointer img3d);

};

#endif