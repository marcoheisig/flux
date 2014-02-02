#include "mpi.h"
#include "FluidSimulator.hh"
#include "SORSolver.hh"
#include "VTKWriter.hh"
#include <math.h>
#include <iostream>
#include <algorithm>

using namespace std;

template <class T>
inline T sq(T value) {
    return value*value;
}

real absmax(Array<real> & a) {
    real m = 0.0;
    for(int y=a.myYStart(); y<=a.myYEnd(); ++y) {
        for(int x=0; x<a.xSize(); ++x) {
            real t = fabs(a(x,y));
            if(t>m)
                m = t;
        }
    }
    
    MPI_Allreduce(MPI_IN_PLACE, &m, 1, mpi_real, MPI_MAX, MPI_COMM_WORLD);
    
    return m;
}

FluidSimulator::FluidSimulator(const FileReader & conf) :
    grid_(conf), conf_(conf), sor(conf) {
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs_);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank_);
}
     
void FluidSimulator::computeFG() {
    const real ReInv = 1.0/conf_.getRealParameter("Re");
    const real gx = conf_.getRealParameter("GX");
    const real gy = conf_.getRealParameter("GY");
    const real gamma = conf_.getRealParameter("gamma");
    
    const real dx = grid_.dx();
    const real dy = grid_.dy();
    const real dxInv = 1.0/dx;
    const real dyInv = 1.0/dy;
    
    const int imax = conf_.getIntParameter("imax");
    const int jmax = conf_.getIntParameter("jmax");
    
    Array<real> & u = grid_.u();
    Array<real> & v = grid_.v();
    Array<real> & f = grid_.f();
    Array<real> & g = grid_.g();
    
    // Setting boundary values
    for(int j=1; j<=jmax; ++j) {
        f(0,j) = u(0,j);
        f(imax,j) = u(imax,j);
    }
    for(int i=1; i<=imax; ++i) {
        g(i,0) = v(i,0);
        g(i,jmax) = v(i,jmax);
    }
    
    // Calculating internal values
    for(int j=max(1, grid_.f().myYStart()); j<=min(jmax, grid_.g().myYEnd()); ++j) {
    //for(int j=1; j<=jmax; ++j) {
        for(int i=1; i<imax; ++i) {
            if(!grid_.isFluid(i,j))
                continue;
            // Update F:
            const real d2udx2 = (grid_.u(i,j, EAST)-2.0*u(i,j)+grid_.u(i,j, WEST))*sq(dxInv);
            const real d2udy2 = (grid_.u(i,j, NORTH)-2.0*u(i,j)+grid_.u(i,j, SOUTH))*sq(dyInv);
            
            const real du2dx = dxInv*( sq(u(i,j)+grid_.u(i,j, EAST))*0.25 - sq(grid_.u(i,j, WEST)+u(i,j))*0.25) + \
                gamma*dxInv*(fabs(u(i,j)+grid_.u(i,j, EAST))*(u(i,j)-grid_.u(i,j, EAST))*0.25 - fabs(grid_.u(i,j, WEST)+u(i,j))*(grid_.u(i,j, WEST)-u(i,j))*0.25);
            const real duvdy = dyInv*((v(i,j)+grid_.v(i,j, EAST))*(u(i,j)+grid_.u(i,j, NORTH))*0.25 - (grid_.v(i,j, SOUTH)+grid_.v(i+1,j, SOUTH)/*v(i+1,j-1)*/)*(grid_.u(i,j, SOUTH)+u(i,j))*0.25) + \
                gamma*dyInv*( fabs(v(i,j)+grid_.v(i,j, EAST)) * (u(i,j)-grid_.u(i,j, NORTH)) * 0.25 - fabs(grid_.v(i,j, SOUTH)+grid_.v(i+1,j, SOUTH)/*v(i+1,j-1)*/) * (grid_.u(i,j, SOUTH)-u(i,j)) * 0.25);
            
            grid_.f()(i,j) = u(i,j)+dt*(ReInv*(d2udx2+d2udy2)-du2dx-duvdy+gx);
        }
    }
    
    for(int j=max(1, grid_.g().myYStart()); j<=min(jmax-1, grid_.g().myYEnd()); ++j) {
    //for(int j=1; j<jmax; ++j) {
        for(int i=1; i<imax+1; ++i) {
            if(!grid_.isFluid(i,j))
                continue;
            // Update G:
            const real d2vdx2 = (grid_.v(i,j, EAST)-2.0*v(i,j)+grid_.v(i,j, WEST))/sq(dx);
            const real d2vdy2 = (grid_.v(i,j, NORTH)-2.0*v(i,j)+grid_.v(i,j, SOUTH))/sq(dy);
        
            const real duvdx = dxInv*((u(i,j)+grid_.u(i,j, NORTH))*(v(i,j)+grid_.v(i,j, EAST))*0.25 - (grid_.u(i,j, WEST)+grid_.u(i-1,j, NORTH)/*u(i-1,j+1)*/)*(grid_.v(i,j, WEST)+v(i,j))*0.25) + \
                gamma*dxInv*( fabs(u(i,j)+grid_.u(i,j, NORTH)) * (v(i,j)-grid_.v(i,j, EAST)) * 0.25 - fabs(grid_.u(i,j, WEST)+grid_.u(i-1,j, NORTH)/*u(i-1,j+1)*/) * (grid_.v(i,j, WEST)-v(i,j)) * 0.25);
            const real dv2dy = dyInv*( sq(v(i,j)+grid_.v(i,j, NORTH))*0.25 - sq(grid_.v(i,j, SOUTH)+v(i,j))*0.25) + \
                gamma*dyInv*(fabs(v(i,j)+grid_.v(i,j, NORTH))*(v(i,j)-grid_.v(i,j, NORTH))*0.25 - fabs(grid_.v(i,j, SOUTH)+v(i,j))*(grid_.v(i,j, SOUTH)-v(i,j))*0.25);
        
            grid_.g()(i,j) = v(i,j)+dt*(ReInv*(d2vdx2+d2vdy2)-duvdx-dv2dy+gy);
        }
    }
    
    // Exchange ghost layer of G in south direction (only g(i,j-1) is needed)
    // required for composeRHS and updateVelocities
    grid_.g().syncGhostLayer(true, true); // only send_north
}

void FluidSimulator::simulate(real duration) {
    // Current time and iteration count
    real t = 0.0;
    int n = 0;
    
    // VTK Writer
    VTKWriter vtkWriter(grid_, conf_.getStringParameter("name"), true, true);
    
    // Initialize U, V and P
    for(int i=0; i<grid_.u().xSize(); ++i)
        for(int j=0; j<grid_.u().ySize(); ++j)
            grid_.u()(i,j) = conf_.getRealParameter("U_INIT");
    
    for(int i=0; i<grid_.v().xSize(); ++i)
        for(int j=0; j<grid_.v().ySize(); ++j)
            grid_.v()(i,j) = conf_.getRealParameter("V_INIT");
    
    for(int i=0; i<grid_.p().xSize(); ++i)
        for(int j=0; j<grid_.p().ySize(); ++j)
            grid_.p()(i,j) = conf_.getRealParameter("P_INIT");
    
    while(t <= duration) {
        // Select dt
        determineNextDT(0.0);
        
        // Set boundary conditions
        refreshBoundaries();
        
        // write vtk file
        if(n % conf_.getIntParameter("outputinterval") == 0) {
            grid_.u().allgather(); // This should be gather(), but does not terminate
            grid_.v().allgather(); // This should be gather(), but does not terminate
            grid_.p().allgather(); // This should be gather(), but does not terminate
            
            if(rank_ == 0)
                vtkWriter.write();
        }
        
        // Recalculate F and G
        computeFG();
        
        // Compute right-hand side
        composeRHS();
        
        // Normalize pressure
        if(n % conf_.getIntParameter("normalizationfrequency") == 0)
            normalize(grid_.p());
        
        // SOR
        if(!sor.solve(grid_)) {
            cerr << "SOR has failed. Aborted." << endl;
            cerr << "Residual was: " << sor.residual(grid_) << endl;
            return;
        }
        
        // Compute u and v
        updateVelocities();
        
        // Increase time and iteration count
        t += dt;
        n++;
    }
}

void FluidSimulator::simulateTimeStepCount(int nrOfTimeSteps) {
    // Current time and iteration count
    real t = 0.0;
    int n = 0;
    
    // VTK Writer
    VTKWriter vtkWriter(grid_, conf_.getStringParameter("name"), true, true);
    
    // Initialize U, V and P
    for(int i=0; i<grid_.u().xSize(); ++i)
        for(int j=0; j<grid_.u().ySize(); ++j) {
            // BACKSTEP SPECIAL!
            if(grid_.isFluid(1,j)) {
                grid_.u()(i,j) = 1.0;
            } else {
               grid_.u()(i,j) = 0.0;
             }
            grid_.u()(i,j) = conf_.getRealParameter("U_INIT");
        }
    
    for(int i=0; i<grid_.v().xSize(); ++i)
        for(int j=0; j<grid_.v().ySize(); ++j)
            grid_.v()(i,j) = conf_.getRealParameter("V_INIT");
    
    for(int i=0; i<grid_.p().xSize(); ++i)
        for(int j=0; j<grid_.p().ySize(); ++j)
            grid_.p()(i,j) = conf_.getRealParameter("P_INIT");

    while(n < nrOfTimeSteps) {
        // Select dt
        determineNextDT(0.0);
        
        if( grid_.p().rank_ == 0)
            cout << "New timestep length: " << dt << endl;

        
        // Set boundary conditions
        refreshBoundaries();
        
        // write vtk file
        if(n % conf_.getIntParameter("outputinterval") == 0) {
            grid_.u().allgather(); // This should be gather(), but does not terminate
            grid_.v().allgather(); // This should be gather(), but does not terminate
            grid_.p().allgather(); // This should be gather(), but does not terminate
            
            if(rank_ == 0)
                vtkWriter.write();
        }
        
        // Recalculate F and G
        computeFG();
        
        // Compute right-hand side
        composeRHS();
        
        // Normalize pressure
        if(n % conf_.getIntParameter("normalizationfrequency") == 0)
            normalize(grid_.p());
        
        // SOR
        sor.solve(grid_);
        
        // Compute u and v
        updateVelocities();
        
        // Increase time and iteration count
        t += dt;
        n++;
    }
    cout << "["<<rank_<<"]" << "Finished after " << n << " timesteps" << endl;
    cout << "["<<rank_<<"]" << "Simulated time: " << t << endl;
}

void  FluidSimulator::composeRHS() {
    const real dtInv = 1.0/dt;
    const real dxInv = 1.0/grid_.dx();
    const real dyInv = 1.0/grid_.dy();
    
    Array<real> & f = grid_.f();
    Array<real> & g = grid_.g();
    
    const int imax = conf_.getIntParameter("imax");
    const int jmax = conf_.getIntParameter("jmax");

    //for(int j=1; j<=jmax; ++j)
    for(int j=max(1, grid_.rhs().myYStart()); j<=min(jmax, grid_.rhs().myYEnd()); ++j)
        for(int i=1; i<=imax; ++i)
            if(grid_.isFluid(i,j))
                grid_.rhs()(i,j) = dtInv*(
                    (f(i,j)-f(i-1,j))*dxInv + (g(i,j)-g(i,j-1))*dyInv);
    

    // grid_.rhs().allgather();
    grid_.rhs().syncGhostLayer(true, true);
    normalize(grid_.rhs());
    

}

void  FluidSimulator::updateVelocities() {
    const real dtdx = dt/grid_.dx();
    const real dtdy = dt/grid_.dy();
    
    const int imax = conf_.getIntParameter("imax");
    const int jmax = conf_.getIntParameter("jmax");
    
    Array<real> & f = grid_.f();
    Array<real> & g = grid_.g();
    Array<real> & u = grid_.u();
    Array<real> & v = grid_.v();
    Array<real> & p = grid_.p();
    
    // Horizontal velocities
    for(int j=max(1, grid_.u().myYStart()); j<=min(jmax, grid_.u().myYEnd()); ++j)
    //for(int j=1; j<=jmax; ++j)
        for(int i=1; i<imax; ++i)
            if(grid_.isFluid(i,j))
                u(i,j) = f(i,j) - dtdx*(grid_.p(i,j, EAST)-p(i,j));
    
    // Vertical velocities
    for(int j=max(1, grid_.v().myYStart()); j<=min(jmax-1,grid_.v().myYEnd()); ++j)
    //for(int j=1; j<jmax; ++j)
        for(int i=1; i<=imax; ++i)
            if(grid_.isFluid(i,j))
                v(i,j) = g(i,j) - dtdy*(grid_.p(i,j, NORTH)-p(i,j));

    grid.u().syncGhostLayers(true, true);
    grid.v().syncGhostLayers(true, true);
}

void FluidSimulator::determineNextDT(real const & limit) {
    const real safetyfactor = conf_.getRealParameter("safetyfactor");
    
    if(safetyfactor <= 0.0) {
        dt = conf_.getRealParameter("dt");
    } else {
        const real umax = absmax(grid_.u());
        const real vmax = absmax(grid_.v());
        const real dx = grid_.dx();
        const real dy = grid_.dy();
        
        const real re = conf_.getRealParameter("Re");
        
        dt = safetyfactor*std::min(re*0.5/(1.0/sq(dx)+1.0/sq(dy)), 
            std::min(dx/umax, dy/vmax));
    }
}

void  FluidSimulator::refreshBoundaries() {
    const int imax = conf_.getIntParameter("imax");
    const int jmax = conf_.getIntParameter("jmax");
    
    Array<real> & u = grid_.u();
    Array<real> & v = grid_.v();
    
    // SOUTH (x, 0)
    if(conf_.getStringParameter("boundary_condition_S") == "noslip") {
        // NOSLIP
        for(int i=1; i<=imax; ++i)
            u(i,0) = conf_.getRealParameter("boundary_velocity_S")*2.0-u(i,1);
    } else if(conf_.getStringParameter("boundary_condition_S") == "inflow") {
        // INFLOW
        for(int i=1; i<=imax; ++i)
            v(i,0) = conf_.getRealParameter("boundary_velocity_S")*2.0-v(i,1);
    } else if(conf_.getStringParameter("boundary_condition_S") == "outflow") {
        // OUTFLOW
        for(int i=1; i<=imax; ++i)
            v(i,0) = v(i,1);
    }
    
    // NORTH (x, jmax)
    if(conf_.getStringParameter("boundary_condition_N") == "noslip") {
        // NOSLIP
        for(int i=1; i<=imax; ++i)
            u(i,jmax+1) = conf_.getRealParameter("boundary_velocity_N")*2.0-u(i,jmax);
    } else if(conf_.getStringParameter("boundary_condition_N") == "inflow") {
        // INFLOW
        for(int i=1; i<=imax; ++i)
            v(i,jmax) = conf_.getRealParameter("boundary_velocity_N")*2.0-v(i,jmax-1);
    } else if(conf_.getStringParameter("boundary_condition_N") == "outflow") {
        // OUTFLOW
        for(int i=1; i<=imax; ++i)
            v(i,jmax) = v(i,jmax-1);
    }
    
    // WEST (0, y)
    if(conf_.getStringParameter("boundary_condition_W") == "noslip") {
        // NOSLIP
        for(int j=1; j<=jmax; ++j)
            v(0,j) = conf_.getRealParameter("boundary_velocity_W")*2.0-v(1,j);
    } else if(conf_.getStringParameter("boundary_condition_W") == "inflow") {
        // INFLOW
        for(int j=1; j<=jmax; ++j)
            u(0,j) = conf_.getRealParameter("boundary_velocity_W")*2.0-u(1,j);
    } else if(conf_.getStringParameter("boundary_condition_W") == "outflow") {
        // OUTFLOW
        for(int j=1; j<=jmax; ++j)
            u(0,j) = u(1,j);
    }
    
    // EAST (imax+1, j)
    if(conf_.getStringParameter("boundary_condition_E") == "noslip") {
        // NOSLIP
        for(int j=1; j<=jmax; ++j)
            v(imax+1,j) = conf_.getRealParameter("boundary_velocity_E")*2.0-v(imax,j);
    } else if(conf_.getStringParameter("boundary_condition_E") == "inflow") {
        // NOSLIP
        for(int j=1; j<=jmax; ++j)
            u(imax,j) = conf_.getRealParameter("boundary_velocity_E")*2.0-u(imax-1,j);
    } else if(conf_.getStringParameter("boundary_condition_E") == "outflow") {
        // OUTFLOW
        for(int j=1; j<=jmax; ++j)
            u(imax,j) = u(imax-1,j);
    }
}

void FluidSimulator::normalize(Array<real> &a) {
    // Find current average
    real avg = 0.0;
    for(int j=max(1, a.myYStart()); j <= min(a.ySize()-1, a.myYEnd()); j++) {
        for(int i=1; i < a.xSize()-1; i++) {
            if(grid_.isFluid(i,j)) {
                avg += a(i,j);
            }
        }
    }
    MPI_Allreduce(MPI_IN_PLACE, &avg, 1, mpi_real, MPI_SUM, MPI_COMM_WORLD);
    avg /= grid_.numFluid;
    
    // Recenter pressure arount zero
    for(int j=max(1, a.myYStart()); j <= min(a.ySize()-1, a.myYEnd()); j++) {
        for(int i=1; i < a.xSize()-1; i++) {
            if(grid_.isFluid(i,j)) {
                a(i,j) -= avg;
            }
        }
    }
    
    a.syncGhostLayer();
    // a.allgather(); // Can be replaced with ghost layver sync once solver is parallelized
}
