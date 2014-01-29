cmake_minimum_required(VERSION 2.8)

find_package( Qt4 REQUIRED )
set( QT_USE_QTOPENGL TRUE )
include( ${QT_USE_FILE} )
include( QtVisualization/AddQtFolders.cmake )

add_qt_folders ( VISUALIZATION_FILES "QtVisualization" )

include_directories( . "QtVisualization" )
set ( OWN_SOURCES  FileReader.cc Debug.cc main.cc SORSolver.cc StaggeredGrid.cc Array.cc FluidSimulator.cc VTKWriter.cc GrayScaleImage.cc lodepng.cc) # and probably many more...

add_executable( nusif ${OWN_SOURCES}  ${VISUALIZATION_FILES}  )
target_link_libraries( nusif ${QT_LIBRARIES}  )
