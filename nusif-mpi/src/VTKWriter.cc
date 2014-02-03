#include "VTKWriter.hh"
#include "Debug.hh"

#include <fstream>
#include <sstream>
#include <iomanip>


template<typename T> struct RealTypeToString         {};
template<>           struct RealTypeToString<float>  { static const char * str; };
template<>           struct RealTypeToString<double> { static const char * str; };

const char * RealTypeToString<float>::str  = "float";
const char * RealTypeToString<double>::str = "double";



VTKWriter::VTKWriter(  const StaggeredGrid & grid, const std::string & basename,
                       bool writePressure,
                       bool writeVelocity,
                       bool writeFG,
                       bool writeObstacles,
                       bool writeRHS)
    : grid_(grid), baseName_( basename ),
      writePressure_(writePressure),
      writeVelocity_(writeVelocity),
      writeFG_(writeFG),
      writeObstacles_(writeObstacles),
      writeRHS_(writeRHS),
      counter_ (0 )
{
    ASSERT_MSG( writePressure_ || writeVelocity_ , "VTK Writer has to write at least velocity or pressure" );

    std::stringstream sstream;
    sstream << "# vtk DataFile Version 4.0\n";
    sstream << "Nusif VTK output\n";
    sstream << "ASCII\n";
    sstream << "DATASET STRUCTURED_POINTS\n";

    sstream << "DIMENSIONS " << grid_.xSize() << " " << grid_.ySize() << " 1\n";
    sstream << "ORIGIN 0 0 0 \n";
    sstream << "SPACING " << grid_.dx() << " " << grid_.dy() << " 1\n";
    sstream << "POINT_DATA " << grid_.xSize() * grid_.ySize() << " \n" << std::endl;

    header_ = sstream.str();
}

void VTKWriter::write()
{
    std::stringstream fileName;
    fileName << baseName_ << "_" <<  std::setw(4) << std::setfill( '0') << counter_ << ".vtk";
    std::ofstream fileStream ( fileName.str().c_str() );

    fileStream << header_;

    if ( writeVelocity_ ) {
        fileStream << "VECTORS velocity " << RealTypeToString<real>::str << "\n";

        for ( int iy = 0; iy < grid_.ySize(); ++iy )
            for ( int ix = 0; ix < grid_.xSize(); ++ix )
                {
                    int x1  = (0                 == ix) ? 0 : ix - 1;
                    int x2  = ((grid_.xSize()-1) == ix) ? (grid_.xSize()-2) : ix;
                    int y1  = (0                 == iy) ? 0 : iy - 1;
                    int y2  = ((grid_.ySize()-1) == iy) ? (grid_.ySize()-2) : iy;

                    const real u = 0.5 * (grid_.u() (x1, iy) +
                                          grid_.u() (x2, iy));

                    const real v = 0.5 * (grid_.v() (ix, y1) +
                                          grid_.v() (ix, y2));

                    fileStream << u << " " << v << " " << " 0\n";
                }

        fileStream << "\n";
    }

    if ( writePressure_ ) {
        fileStream << "SCALARS pressure " << RealTypeToString<real>::str << " 1\n";
        fileStream << "LOOKUP_TABLE default\n";

        for ( int iy = 0; iy < grid_.ySize(); ++iy)
            for ( int ix = 0; ix < grid_.xSize(); ++ix)
                fileStream << grid_.p()(ix, iy) << "\n";

        fileStream << "\n";
    }

    if ( writeRHS_ ) {
        fileStream << "SCALARS RHS " << RealTypeToString<real>::str << " 1\n";
        fileStream << "LOOKUP_TABLE default\n";

        for(int iy = 0; iy < grid_.ySize(); ++iy)
            for (int ix = 0; ix < grid_.xSize(); ++ix)
                fileStream << grid_.rhs()(ix, iy) << "\n";
        fileStream << "\n";
    }

    if ( writeFG_ ) {
        fileStream << "VECTORS FG " << RealTypeToString<real>::str << "\n";

        for ( int iy = 0; iy < grid_.ySize(); ++iy )
            for ( int ix = 0; ix < grid_.xSize(); ++ix )
                {
                    int x1  = (0                 == ix) ? 0 : ix - 1;
                    int x2  = ((grid_.xSize()-1) == ix) ? (grid_.xSize()-2) : ix;
                    int y1  = (0                 == iy) ? 0 : iy - 1;
                    int y2  = ((grid_.ySize()-1) == iy) ? (grid_.ySize()-2) : iy;

                    const real f = 0.5 * (grid_.f() (x1, iy) +
                                          grid_.f() (x2, iy));

                    const real g = 0.5 * (grid_.g() (ix, y1) +
                                          grid_.g() (ix, y2));

                    fileStream << f << " " << g << " " << " 0\n";
                }

        fileStream << "\n";
    }

    if ( writeObstacles_ ) {
        fileStream << "SCALARS Obstacles " << RealTypeToString<real>::str << " 1\n";
        fileStream << "LOOKUP_TABLE default\n";

        for(int iy = 0; iy < grid_.ySize(); ++iy)
            for (int ix = 0; ix < grid_.xSize(); ++ix)
                fileStream << grid_.flag()(ix, iy) << "\n";
        fileStream << "\n";
    }

    ++counter_;
}
