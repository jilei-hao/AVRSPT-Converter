

add_executable(ModelGen model_generator.cxx)
target_link_libraries(ModelGen PUBLIC ${LIB_EXTERNAL} ${LIB_INTERNAL})
target_include_directories(ModelGen PUBLIC ${LIB_INTERNAL_DIRS})


add_executable(VTKToVTP vtk_to_vtp.cxx)
target_link_libraries(VTKToVTP PUBLIC ${VTK_LIBRARIES})

set(GREEDY_INCLUDE_DIRS 
  ${avrspt-converter_SOURCE_DIR}/greedy/src
  ${avrspt-converter_SOURCE_DIR}/greedy/src/ITKFilters/include
  ${avrspt-converter_SOURCE_DIR}/greedy/src/propagation
)

add_executable(CaseGen case_generator.cxx)
target_link_libraries(CaseGen PUBLIC 
  ${LIB_EXTERNAL}
  ${LIB_INTERNAL}
  propagationapi 
  greedyapi
)
target_include_directories(CaseGen PUBLIC ${GREEDY_INCLUDE_DIRS})

add_executable(VolumeGenerator volume_generator.cxx)
target_link_libraries(VolumeGenerator PUBLIC ${LIB_EXTERNAL})

add_executable(TaubinSmooth smooth_taubin.cxx)
target_link_libraries(TaubinSmooth PUBLIC ${LIB_EXTERNAL} ${LIB_INTERNAL})
target_include_directories(TaubinSmooth PUBLIC ${LIB_INTERNAL_DIRS})

add_executable(LaplacianSmooth smooth_laplacian.cxx)
target_link_libraries(LaplacianSmooth PUBLIC ${LIB_EXTERNAL} ${LIB_INTERNAL})
target_include_directories(LaplacianSmooth PUBLIC ${LIB_INTERNAL_DIRS})

add_executable(QuadricDecimate decimate_quadric.cxx)
target_link_libraries(QuadricDecimate PUBLIC ${LIB_EXTERNAL} ${LIB_INTERNAL})
target_include_directories(QuadricDecimate PUBLIC ${LIB_INTERNAL_DIRS})


add_executable(VTIProbe vti_probe.cxx)
target_link_libraries(VTIProbe PUBLIC ${LIB_EXTERNAL})

add_executable(vti2nii vti_to_nii.cxx)
target_link_libraries(vti2nii PUBLIC ${LIB_EXTERNAL})

add_executable(nii2vti nii_to_vti.cxx)
target_link_libraries(nii2vti PUBLIC ${LIB_EXTERNAL})