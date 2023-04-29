set(components_hdr 
  image_to_mesh.h
)

set(components_src 
  image_to_mesh.cxx
)

add_library(Components ${components_src})
target_link_libraries(Components PUBLIC 
  ${VTK_LIBRARIES} 
  ${ITK_LIBRARIES}
)

add_executable(Test_Components main.cxx)
target_link_libraries(Test_Components PRIVATE Components)