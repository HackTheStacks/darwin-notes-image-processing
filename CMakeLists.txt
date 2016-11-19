cmake_minimum_required(VERSION 2.8.9 FATAL_ERROR)
if(COMMAND CMAKE_POLICY)
  cmake_policy(SET CMP0003 NEW)
endif()

project(DarwinImageProcessing)

find_package(ITK REQUIRED)
include(${ITK_USE_FILE})

add_subdirectory(Segmentation)
add_subdirectory(Visualization)

