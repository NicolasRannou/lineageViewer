cmake_minimum_required(VERSION 2.6)

project(lineageViewer)

find_package(VTK)
if(NOT VTK_DIR)
  message(FATAL_ERROR "Please set VTK_DIR.")
endif()
include(${VTK_USE_FILE})

# use what QVTK built with
set(QT_MOC_EXECUTABLE ${VTK_QT_MOC_EXECUTABLE} CACHE FILEPATH "")
set(QT_UIC_EXECUTABLE ${VTK_QT_UIC_EXECUTABLE} CACHE FILEPATH "")
set(QT_QMAKE_EXECUTABLE ${VTK_QT_QMAKE_EXECUTABLE} CACHE FILEPATH "")
set(DESIRED_QT_VERSION ${VTK_DESIRED_QT_VERSION} CACHE FILEPATH "")

find_package(Qt4 REQUIRED)
include(${QT_USE_FILE})


# Use the include path and library for Qt that is used by VTK.
include_directories(
  ${QT_INCLUDE_DIR}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
)

# Set your files and resources here
set(Srcs
  main.cxx
  lineageViewer.cxx
  )
set(UIs lineageViewer.ui)
set(Headers lineageViewer.h)

# The rest should just work (sure...)
QT4_WRAP_UI(UISrcs ${UIs})
QT4_WRAP_CPP(MOCSrcs ${Headers} )
QT4_ADD_RESOURCES(ResourceSrcs ${Resources})

source_group("Resources" FILES
  ${UIs}
  ${Resources}
  ${EXE_ICON}
)

source_group("Generated" FILES
  ${UISrcs}
  ${MOCSrcs}
  ${ResourceSrcs}
  ${RCS_SOURCES}
)

add_definitions(-DQT_GUI_LIBS -DQT_CORE_LIB)
set_source_files_properties(${Srcs} PROPERTIES
                            OBJECT_DEPENDS "${UISrcs}")

# It's nice to have the ui in the windows project file...just double click on it
# and designer comes up on that ui file :)
if (${CMAKE_BUILD_TOOL} MATCHES "msdev")
  set (Srcs ${Srcs} ${UIs})
endif()
if (${CMAKE_BUILD_TOOL} MATCHES "devenv")
  set (Srcs ${Srcs} ${UIs})
endif()

add_executable( lineageViewer MACOSX_BUNDLE ${Srcs} ${UISrcs} ${MOCSrcs} ${ResourceSrcs})

target_link_libraries( lineageViewer
  QVTK
  ${QT_LIBRARIES}
  vtkViews
  )

