add_executable(ImageToVTI image_to_vti.cxx)
target_link_libraries(ImageToVTI PUBLIC ${VTK_LIBRARIES})