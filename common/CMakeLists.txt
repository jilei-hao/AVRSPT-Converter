set(CommonTools_Src 
  common_tools.cxx
  image_helpers.cxx
  mesh_helpers.cxx
)

add_library(CommonTools ${CommonTools_Src})
target_link_libraries(CommonTools PUBLIC ${LIB_EXTERNAL})