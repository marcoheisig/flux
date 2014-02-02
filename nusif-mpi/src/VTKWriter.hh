#ifndef VTKFILEWRITER_HH
#define VTKFILEWRITER_HH


#include "StaggeredGrid.hh"



//*******************************************************************************************************************
/*! Writes StaggeredGrid as vtk file

  - vtk files can for example be opened with Paraview ( http://www.paraview.org/ )
  - writes out pressure and/or velocity

  - Usage:
  \code
  VTKWriter vtkWriter ( myGrid, "lidDrivenCavity", true, true );
  // for each timestep:
  vtkWriter.write();
  \endcode
  This creates on file per timestep: "lidDrivenCavity_0001.vtk", ""lidDrivenCavity_0002.vtk" ...

*/
//*******************************************************************************************************************
class VTKWriter
{

public:

    VTKWriter(  const StaggeredGrid & grid, const std::string & basename,
                bool writePressure = true,
                bool writeVelocity = true,
                bool writeFG = true,
                bool writeObstacles = true,
                bool writeRHS = true );

    void write();

private:
    const StaggeredGrid & grid_;
    std::string baseName_;

    const bool writePressure_;
    const bool writeVelocity_;
    const bool writeFG_;
    const bool writeObstacles_;
    const bool writeRHS_;

    int counter_;
    std::string header_;

};



#endif




