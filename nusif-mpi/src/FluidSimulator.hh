#ifndef __FLUID_SIMULATOR_H__
#define __FLUID_SIMULATOR_H__


#include "FileReader.hh"
#include "StaggeredGrid.hh"
#include "SORSolver.hh"

class FluidSimulator
{
  public:
      FluidSimulator( const FileReader & conf );

      /// Simulates a given time-length
      void simulate             ( real duration     );
      void simulateTimeStepCount( int nrOfTimeSteps );


      // Getter functions for the internally stored StaggeredGrid
            StaggeredGrid & grid() { return grid_; };
      const StaggeredGrid & grid() const { return grid_; };

  private:
      void computeFG();
      void composeRHS();
      void updateVelocities();
      void determineNextDT(real const & limit);
      void refreshBoundaries();
      void normalize(Array<real> &a);
      
      StaggeredGrid grid_;
      const FileReader & conf_;
      SORSolver sor;
      real dt;
      int rank_;
      int num_procs_;
};



#endif
