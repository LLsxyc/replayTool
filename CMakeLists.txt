cmake_minimum_required(VERSION 3.5)

project(replayTool LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "/opt/Qt/5.15.2/gcc_64")

set(Qt5_DIR "${CMAKE_PREFIX_PATH}/lib/cmake/Qt5")
set(Qt5Widgets_DIR "${CMAKE_PREFIX_PATH}/lib/cmake/Qt5Widgets")
set(Qt5Charts_DIR "${CMAKE_PREFIX_PATH}/lib/cmake/QtCharts")
set(Qt5Core_DIR "${CMAKE_PREFIX_PATH}/lib/cmake/QtCort")
set(Qt5Gui_DIR "${CMAKE_PREFIX_PATH}/lib/cmake/QtGui")
set(Qt5PrintSupport_DIR "${CMAKE_PREFIX_PATH}/lib/cmake/QtPrintSupport")
set(PROJ4_DIR "${CMAKE_CURRENT_SOURCE_DIR}/install/lib/cmake/proj4")
message("CMAKE_CURRENT_SOURCE_DIR === " ${CMAKE_CURRENT_SOURCE_DIR})
message("PROJ4_DIR === " ${PROJ4_DIR})

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Charts REQUIRED)
find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Gui REQUIRED)
find_package(Qt5 COMPONENTS PrintSupport REQUIRED)

find_package(PROJ4 REQUIRED CONFIG)

# include_directories(
#  ${CMAKE_CURRENT_SOURCE_DIR}/install
#  #	${CMAKE_CURRENT_SOURCE_DIR}/include
# )

#aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/src SOURCE_SRC)

#list (APPEND REPLAYTOOL_SRC

#	${SOURCE_SRC}
#  src/widget.ui

#  include/chartview.h
#  include/widget.h
#  include/obj.h
#  include/convert.h
#  include/qcustomplot.h
#)
#ADD_EXECUTABLE(${PROJECT_NAME} ${REPLAYTOOL_SRC})
add_executable(${PROJECT_NAME}
 main.cpp
 widget.cpp
 widget.ui
 chartview.cpp
 convert.cpp
 qcustomplot.cpp
 MultiSelectComboBox.cpp
# qcustomtooltip.cpp
# qcustomtooltip.h
 MultiSelectComboBox.h
 qcustomplot.h
 widget.h
 chartview.h
 convert.h
 obj.h
 #  /home/lei/third_libs/PROJ//include
)

target_link_libraries(${PROJECT_NAME} PRIVATE
 Qt5::Widgets
 Qt5::Charts
 Qt5::Core
 Qt5::Gui
 Qt5::PrintSupport
#  ${CMAKE_CURRENT_SOURCE_DIR}/install/lib/libproj.so
  PROJ::proj
)


