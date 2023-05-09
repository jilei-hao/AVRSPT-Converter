set(CommonTools_Src 
  type_defs.hxx
  common_tools.cxx
  image_helpers.cxx
  mesh_helpers.cxx
)

add_library(CommonTools ${CommonTools_Src})
target_link_libraries(CommonTools PUBLIC ${LIB_EXTERNAL})

add_executable(TestCommon test_common.cxx)
target_link_libraries(TestCommon PRIVATE CommonTools ${LIB_EXTERNAL})