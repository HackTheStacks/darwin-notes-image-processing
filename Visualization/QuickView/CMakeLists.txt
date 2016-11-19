if (ITKVtkGlue_LOADED)
  find_package(VTK REQUIRED)
  include(${VTK_USE_FILE})
else()
  find_package(ItkVtkGlue REQUIRED)
  include(${ItkVtkGlue_USE_FILE})
  set(Glue ItkVtkGlue)
endif()
 
add_executable(QuickView MACOSX_BUNDLE QuickView.cxx)
target_link_libraries(QuickView
  ${Glue}  ${VTK_LIBRARIES} ${ITK_LIBRARIES})
