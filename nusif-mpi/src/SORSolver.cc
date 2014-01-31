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
    Array<real> &p = grid.p();
    Array<real> &rhs = grid.rhs();
    
    const real dx2inv = 1.0/(grid.dx()*grid.dx());
    const real dy2inv = 1.0/(grid.dy()*grid.dy());
    
    const real c = omg_*1.0/(2.0*dx2inv+2.0*dy2inv);

    setBoundary(grid);
        
    real res = residual(grid);

#ifndef NDEBUG
    cout << "Initial residual: " << res << endl;
#endif
    int iter=0;
    while(iter++ <= itermax_ && res > eps_) {
        // Perform one SOR iteration
        # pragma omp parallel for
        for(int i=1; i<=imax_; i++) {
            for(int j=1; j<=jmax_; j++) {
                if(grid.isFluid(i,j))
                    p(i,j) = (1-omg_)*p(i,j)+c*(dx2inv*(grid.p(i,j, EAST)+grid.p(i,j, WEST))+dy2inv*(grid.p(i,j, NORTH)+grid.p(i,j, SOUTH))-rhs(i,j));
            }
        }
        
        setBoundary(grid);
        
        if(iter % checkfrequency_ == 0) {
            res = residual(grid);
        
        grid.synchronizeGhostPressure();
#ifndef NDEBUG
    //cout << "Residual: " << res << endl;
#endif
        }
    }
    
#ifndef NDEBUG
    cout << iter << " iterations" << endl;
    cout << scientific << "final residual: " << res << " (eps: "<<eps_<<")" << endl;
#endif
    
    return res <= eps_;
}

void SORSolver::setBoundary(StaggeredGrid & grid) {
    Array<real> &p = grid.p();
    
    // West and East
    for(int j=1; j <= jmax_; j++) {
        p(0,j) = p(1,j);
        p(imax_+1,j) = p(imax_,j);
    }
    
    // North and South
    for(int i=1; i <= imax_; i++) {
        p(i,0) = p(i,1);
        p(i,jmax_+1) = p(i,jmax_);
    }
}

real SORSolver::residual(StaggeredGrid & grid) {
    Array<real> &p = grid.p();
    Array<real> &rhs = grid.rhs();
    
    const real dx2inv = 1.0/(grid.dx()*grid.dx());
    const real dy2inv = 1.0/(grid.dy()*grid.dy());
    
    real sum = 0.0;
    
    for(int i=1; i<=imax_; i++) {
        for(int j=1; j<=jmax_; j++) {
            if(grid.isFluid(i,j)) {
                real r = dx2inv*(p(i+1,j)-2.0*p(i,j)+p(i-1,j)) + 
                    dy2inv*(grid.p(i,j, NORTH)-2.0*p(i,j)+grid.p(i,j, SOUTH))-rhs(i,j);
                sum += r*r;
            }
        }
    }
    
    return sqrt(sum/(imax_*jmax_));
}