cmake_minimum_required(VERSION 2.8)

set ( OWN_SOURCES  FileReader.cc Debug.cc main.cc SORSolver.cc StaggeredGrid.cc Array.cc FluidSimulator.cc VTKWriter.cc GrayScaleImage.cc lodepng.cc) # and probably many more...

add_executable( nusif ${OWN_SOURCES}  )
