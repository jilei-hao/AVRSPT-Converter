add_executable(ImageToVTI image_to_vti.cxx)
target_link_libraries(ImageToVTI PUBLIC ${VTK_LIBRARIES})

add_executable(ModelGen model_generator.cxx)
target_link_libraries(ModelGen PUBLIC ${VTK_LIBRARIES})

add_executable(ModelProcessor model_processor.cxx)
target_link_libraries(ModelProcessor PUBLIC ${VTK_LIBRARIES})

add_executable(VTKToVTP vtk_to_vtp.cxx)
target_link_libraries(VTKToVTP PUBLIC ${VTK_LIBRARIES})

set(GREEDY_INCLUDE_DIRS 
  ${avrspt-converter_SOURCE_DIR}/greedy/src
  ${avrspt-converter_SOURCE_DIR}/greedy/src/ITKFilters/include
  ${avrspt-converter_SOURCE_DIR}/greedy/src/propagation
)

add_executable(CaseGen case_generator.cxx)
target_link_libraries(CaseGen PUBLIC 
  ${VTK_LIBRARIES} 
  ${ITK_LIBRARIES} 
  propagationapi 
  greedyapi
)
target_include_directories(CaseGen PUBLIC ${GREEDY_INCLUDE_DIRS})

add_executable(VolumeGenerator volume_generator.cxx)
target_link_libraries(VolumeGenerator PUBLIC ${VTK_LIBRARIES} ${ITK_LIBRARIES})


add_executable(TaubinSmooth smooth_taubin.cxx)
target_link_libraries(TaubinSmooth PUBLIC ${VTK_LIBRARIES} ${ITK_LIBRARIES})

add_executable(LaplacianSmooth smooth_laplacian.cxx)
target_link_libraries(LaplacianSmooth PUBLIC ${VTK_LIBRARIES} ${ITK_LIBRARIES})