
#include <mpi.h>
#include <iostream>
#include "Types.hh"
#include "FileReader.hh"
#include "StaggeredGrid.hh"
#include "SORSolver.hh"
#include "math.h"
#include <cmath>

using namespace std;

SORSolver::SORSolver(int imax, int jmax, int itermax, real eps, int omg, int checkfrequency) :
    imax_(imax), jmax_(jmax), itermax_(itermax), eps_(eps), omg_(omg), checkfrequency_(checkfrequency) {
}

SORSolver::SORSolver(const FileReader & configuration) : 
    imax_(configuration.getIntParameter("imax")),
    jmax_(configuration.getIntParameter("jmax")),
    itermax_(configuration.getIntParameter("itermax")),
    eps_(configuration.getRealParameter("eps")),
    omg_(configuration.getRealParameter("omg")),
    checkfrequency_(configuration.getIntParameter("checkfrequency")) {
}

bool SORSolver::solve(StaggeredGrid & grid) {
    
    Array<real> *p_src = & (grid.p());
    Array<real> *p_dst = & (grid.p2());

    Array<real> &rhs = grid.rhs();
    
    const real dx2inv = 1.0/(grid.dx()*grid.dx());
    const real dy2inv = 1.0/(grid.dy()*grid.dy());
    
    const real c = 1.0/(2.0*dx2inv+2.0*dy2inv);


    real res = residual(*p_src, grid);


    int iter=0;
    while(iter++ <= itermax_ && res > eps_) {

                        
        for(int j = max( 1, p_src->myYStart() );
            j <= min( p_src->myYEnd(), jmax_); j++) {
            for(int i=1; i<=imax_; i++) {
                if(grid.isFluid(i,j))
                    (*p_dst)(i,j) = c * ( dx2inv*( grid.p(p_src, i,j, EAST) + 
                                                   grid.p(p_src, i,j, WEST)) + 
                                          dy2inv*( grid.p(p_src, i,j, NORTH) + 
                                                   grid.p(p_src, i,j, SOUTH))
                                          -rhs(i,j));

            }
        }
        
        setBoundary(p_dst);
        p_dst->syncGhostLayer();
        
        if(iter % checkfrequency_ == 0) {
            res = residual( *p_dst, grid );
        }
        swap(p_src, p_dst);
    }

    if(p_dst->rank_ == 0) {
        cout << "Residual at " << res << " after " << iter << " iterations\n";
    } 
    
    return res <= eps_;
}

void SORSolver::setBoundary(Array<real> * p) {
     
    // West and East
    for(int j=1; j <= jmax_; j++) {
        (*p)(0,j) = (*p)(1,j);
        (*p)(imax_+1,j) = (*p)(imax_,j);
    }
    
    // North and South
    for(int i=1; i <= imax_; i++) {
        (*p)(i,0) = (*p)(i,1);
        (*p)(i,jmax_+1) = (*p)(i,jmax_);
    }
}

real SORSolver::residual(StaggeredGrid& grid) {
    return residual( grid.p(), grid );
}

real SORSolver::residual(Array<real> &p, StaggeredGrid& grid) {
    Array<double>& rhs = grid.rhs();
    
    const real dx2inv = 1.0/(grid.dx()*grid.dx());
    const real dy2inv = 1.0/(grid.dy()*grid.dy());
    
    real sum = 0.0;
    
    p.syncGhostLayer();

    for(int j=max(1, p.myYStart()); j<=min(jmax_, p.myYEnd()); j++) {
    //for(int j=1; j<=jmax_; j++) {
        for(int i=1; i<=imax_; i++) {
            if(grid.isFluid(i,j)) {
                real r = 
                    dx2inv*( grid.p(&p, i,j, EAST)
                             - 2.0*p(i,j)
                             +grid.p(&p, i,j, WEST) ) + 
                    dy2inv* ( grid.p(&p, i,j, NORTH) 
                              - 2.0*p( i,j) 
                              + grid.p(&p, i,j, SOUTH))
                    -rhs(i,j);
                sum += r*r;
            }
        }
    }
    
    MPI_Allreduce(MPI_IN_PLACE, &sum, 1, mpi_real, MPI_SUM, MPI_COMM_WORLD);
    
    return sqrt(sum/(imax_*jmax_));
}
