cmake_minimum_required(VERSION 3.12 FATAL_ERROR)

project(avrspt-converter)

# C++11 required
set (CMAKE_CXX_STANDARD 11)

find_package(VTK COMPONENTS
  CommonCore
  ImagingGeneral
  InteractionStyle
  IOExport
  IOLegacy
  IOImage
  IOXML
  FiltersCore
  RenderingCore
  RenderingContextOpenGL2
  RenderingFreeType
  RenderingGL2PSOpenGL2
  RenderingOpenGL2
)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

if (NOT VTK_FOUND)
  message(FATAL_ERROR "Unable to find the VTK build folder.")
endif()

if (NOT ITK_FOUND)
  message(FATAL_ERROR "Unable to find the ITK build folder.")
endif()

set(LIB_INTERNAL_DIRS 
  ${avrspt-converter_SOURCE_DIR}/common
)

set(LIB_EXTERNAL 
  ${ITK_LIBRARIES} 
  ${VTK_LIBRARIES}
)

# configure greedy
set(BUILD_CLI OFF)
add_subdirectory(greedy)

# internal libraries
add_subdirectory(common)

set(LIB_INTERNAL 
  CommonTools
)

add_subdirectory(components)

add_subdirectory(converters bin)

