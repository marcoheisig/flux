#ifndef SOR_SOLVER_HH
#define SOR_SOLVER_HH

#include "StaggeredGrid.hh"
#include "FileReader.hh"

class SORSolver
{
public:
    // Constructor to manually create SORSolver
    SORSolver (int imax, int jmax, int itermax, real eps, int omg, int checkfrequency);

    // Constructor to create a SORSolver from a parsed configuration file
    SORSolver ( const FileReader & configuration );

    // solve the pressure equation on the staggered grid
    bool solve( StaggeredGrid & grid );

    // Calculates residual of grid
    real residual( StaggeredGrid & grid );
    
    void setBoundary(StaggeredGrid & grid);
    
    void normalizePressure(StaggeredGrid & grid);

private:
    const int imax_, jmax_;
    const int itermax_;
    const real eps_;
    const real omg_;
    const int checkfrequency_;
};

#endif //SOR_SOLVER_HH




