#include "mesh_helpers.h"

#include <itkImage.h>
#include <vtkPolyData.h>
#include <vtkPoints.h>
#include <vtkUnsignedShortArray.h>
#include <vnl/vnl_inverse.h>

MeshHelpers
::MeshHelpers()
{

}

MeshHelpers
::~MeshHelpers()
{

}

vnl_matrix_fixed<double, 4, 4> 
MeshHelpers
::ConstructNiftiSform(vnl_matrix<double> m_dir, vnl_vector<double> v_origin,
  vnl_vector<double> v_spacing)
{
  // Set the NIFTI/RAS transform
  vnl_matrix<double> m_ras_matrix;
  vnl_diag_matrix<double> m_scale, m_lps_to_ras;
  vnl_vector<double> v_ras_offset;

  // Compute the matrix
  m_scale.set(v_spacing);
  m_lps_to_ras.set(vnl_vector<double>(3, 1.0));
  m_lps_to_ras[0] = -1;
  m_lps_to_ras[1] = -1;
  m_ras_matrix = m_lps_to_ras * m_dir * m_scale;

  // Compute the vector
  v_ras_offset = m_lps_to_ras * v_origin;

  // Create the larger matrix
  vnl_vector<double> vcol(4, 1.0);
  vcol.update(v_ras_offset);

  vnl_matrix_fixed<double,4,4> m_sform;
  m_sform.set_identity();
  m_sform.update(m_ras_matrix);
  m_sform.set_column(3, vcol);
  return m_sform;
}

vnl_matrix_fixed<double, 4, 4>
MeshHelpers
::ConstructVTKtoNiftiTransform(vnl_matrix<double> m_dir, vnl_vector<double> v_origin, 
  vnl_vector<double> v_spacing)
{
  std::cout << "---- [ConstructVTKtoNiftiTransform] dir:\n " << m_dir << "\norg: " 
            << v_origin << "\nspc:" << v_spacing << std::endl;

  vnl_matrix_fixed<double,4,4> vox2nii = ConstructNiftiSform(m_dir, v_origin, v_spacing);
  vnl_matrix_fixed<double,4,4> vtk2vox;
  vtk2vox.set_identity();
  for(size_t i = 0; i < 3; i++)
    {
    vtk2vox(i,i) = 1.0 / v_spacing[i];
    vtk2vox(i,3) = - v_origin[i] / v_spacing[i];
    }

  std::cout << "---- vox2nii: \n" << vox2nii << "\nvtk2vox: \n" << vtk2vox << std::endl;
  return vox2nii * vtk2vox;
}

vtkSmartPointer<vtkTransform> 
MeshHelpers::
getVTKToNiftiTransform(itk::ImageBase<3>::Pointer img3d)
{
  auto vtk2nii = ConstructVTKtoNiftiTransform(
    img3d->GetDirection().GetVnlMatrix().as_ref(),
    img3d->GetOrigin().GetVnlVector(),
    img3d->GetSpacing().GetVnlVector()
  );

  std::cout << "---- [getVTKToNiftiTransform] vtk2nii:" << std::endl;
  vtk2nii.print(std::cout);

  vtkNew<vtkTransform> transform;
  transform->SetMatrix(vtk2nii.data_block());
  transform->Update();

  return transform;
}


void 
MeshHelpers
::mapLabelVoxelToPointData(vtkSmartPointer<vtkPolyData> mesh, 
  itk::Image<uint16_t, 3>::Pointer img3d)
{
  auto points = mesh->GetPoints();

  typedef itk::Image<uint16_t, 3> LabelImage3DType;

  std::cout << "Mesh Print: " << std::endl;
  mesh->Print(std::cout);
  std::cout << std::endl;

  std::cout << "Image Print: " << std::endl;
  img3d->Print(std::cout);
  std::cout << std::endl;

  auto vox2nii = ConstructNiftiSform(
    img3d->GetDirection().GetVnlMatrix().as_ref(),
    img3d->GetOrigin().GetVnlVector(),
    img3d->GetSpacing().GetVnlVector()
  );

  std::cout << "vox2nii: \n" << vox2nii << std::endl;

  auto nii2vox = vnl_inverse(vox2nii);

  std::cout << "nii2vox: \n" << nii2vox << std::endl;

  for (vtkIdType i = 0; i < 10; ++i)
  {
    double meshPos[3];
    points->GetPoint(i, meshPos);

    LabelImage3DType::IndexType imgIndex;
    LabelImage3DType::PointType imgPos;

    vnl_vector_fixed<double, 4> meshPosVector(1.0);

    for (int dim = 0; dim < 3; ++dim)
    {
      meshPosVector[dim] = meshPos[dim];
    }

    meshPosVector = meshPosVector * nii2vox;

    for (int dim = 0; dim < 3; ++dim)
    {
      imgPos[i] = meshPosVector[i];
    }
    
    img3d->TransformPhysicalPointToIndex(imgPos, imgIndex);
    uint16_t pv = img3d->GetPixel(imgIndex);

    std::cout << "MeshPos: [" << meshPos[0] << "," << meshPos[1] << "," << meshPos[2] << "]" 
              << "; ImageInd: [" << imgIndex[0] << "," << imgIndex[1] << "," << imgIndex[2] << "]"
              << "; PixelValue: " << pv
              << std::endl;
  }

}


void 
MeshHelpers
::getVoxelIJKfromMesh(double *meshPos, double *voxPos,
  vnl_vector<double> voxOrigin, vnl_vector<double> voxSpacing)
{

}