add_executable(ImageToVTI image_to_vti.cxx)
target_link_libraries(ImageToVTI PUBLIC ${VTK_LIBRARIES})


add_executable(ModelGen model_generator.cxx)
target_link_libraries(ModelGen PUBLIC ${VTK_LIBRARIES})

add_executable(ModelProcessor model_processor.cxx)
target_link_libraries(ModelProcessor PUBLIC ${VTK_LIBRARIES})

add_executable(VTKToVTP vtk_to_vtp.cxx)
target_link_libraries(VTKToVTP PUBLIC ${VTK_LIBRARIES})