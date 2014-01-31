#include "mpi.h"
#include "Types.hh"
#include "FileReader.hh"
#include "StaggeredGrid.hh"
#include "Array.hh"
#include "GrayScaleImage.hh"
#include <iostream>

int StaggeredGrid::yStartOfBlock(int rank) {
    return blockHeight()*rank;
}

int StaggeredGrid::yEndOfBlock(int rank) {
    // This can be assumed, since we checked for 
    // imax+2 % num_procs_ == 0
    return blockHeight()*(rank+1)-1;
}

int StaggeredGrid::blockHeight() {
    return (ySize_+2)/num_procs_;
}

int StaggeredGrid::blockWidth() {
    return xSize_+2;
}

StaggeredGrid::StaggeredGrid(int xxSize, int yySize, real ddx, real ddy) :
    xSize_(xxSize), ySize_(yySize),
    p_(xxSize, yySize), rhs_(xxSize, yySize), u_(xxSize, yySize), v_(xxSize, yySize), 
    f_(xxSize, yySize), g_(xxSize, yySize), flag_(xxSize, yySize), dx_(ddx), dy_(ddy) {
    flag_.fill(FLUID);
}

StaggeredGrid::StaggeredGrid(const FileReader & configuration) :
    xSize_(configuration.getIntParameter("imax")+2), ySize_(configuration.getIntParameter("jmax")+2),
    p_(configuration.getIntParameter("imax")+2, configuration.getIntParameter("jmax")+2),
    rhs_(configuration.getIntParameter("imax")+2, configuration.getIntParameter("jmax")+2),
    u_(configuration.getIntParameter("imax")+2/* 1 */, configuration.getIntParameter("jmax")+2),
    v_(configuration.getIntParameter("imax")+2, configuration.getIntParameter("jmax")+2/* 1 */),
    f_(configuration.getIntParameter("imax")+2/* 1 */, configuration.getIntParameter("jmax")+2),
    g_(configuration.getIntParameter("imax")+2, configuration.getIntParameter("jmax")+2)/* 1 */,
    flag_(configuration.getIntParameter("imax")+2, configuration.getIntParameter("jmax")+2),
    dx_(configuration.getRealParameter("xlength")/(real)configuration.getIntParameter("imax")),
    dy_(configuration.getRealParameter("ylength")/(real)configuration.getIntParameter("jmax")) {
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs_);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank_);
    
    flag_.fill(FLUID);
    
    // Include obstacles
    if(configuration.getRealParameter("RectangleX1") >= 0.0 && 
       configuration.getRealParameter("RectangleX2") >= 0.0 && 
       configuration.getRealParameter("RectangleY1") >= 0.0 && 
       configuration.getRealParameter("RectangleY2") >= 0.0) {
        createRectangle((int)(configuration.getRealParameter("RectangleX1")/dx_)+1,
                        (int)(configuration.getRealParameter("RectangleY1")/dy_)+1,
                        (int)(configuration.getRealParameter("RectangleX2")/dx_)+1,
                        (int)(configuration.getRealParameter("RectangleY2")/dy_)+1);
    }
    
    if(configuration.getRealParameter("CircleX") >= 0.0 && 
       configuration.getRealParameter("CircleY") >= 0.0 && 
       configuration.getRealParameter("CircleR") > 0.0) {
        createCircle((int)(configuration.getRealParameter("CircleX")/dx_)+1,
                     (int)(configuration.getRealParameter("CircleY")/dy_)+1,
                     (int)(configuration.getRealParameter("CircleR")/dx_));
    }
    
    int imax = configuration.getIntParameter("imax");
    int jmax = configuration.getIntParameter("jmax");
    
    // load obstacles from png
    if(configuration.getStringParameter("ObstacleFile") != "") {
        int threshold = configuration.getIntParameter("threshold");
        
        GrayScaleImage of = GrayScaleImage(configuration.getStringParameter("ObstacleFile"));
        of = of.getResizedImage(imax, jmax);
        
        // Set all cells with value above threshold to solid
        for(int i=1; i<=imax; i++)
            for(int j=1; j<jmax; j++)
                if((int)of.getElement(i, jmax-j) > threshold)
                    flag_(i,j) = SOLID;
    }

    // Write to png file
    GrayScaleImage of = GrayScaleImage(imax, jmax);
    for(int i=1; i<imax; i++)
        for(int j=1; j<jmax; j++) {
            if(isFluid(i, j))
                of.getElement(i-1,jmax-j-1) = 255;
            else
                of.getElement(i-1,jmax-j-1) = 0;
        }
    std::stringstream fileName;
    fileName << configuration.getStringParameter("name") << "_obs.png";
    of.save(fileName.str());
}

void StaggeredGrid::createRectangle(int x1, int y1, int x2, int y2) {
    cout << x1 << " " << y1 << "     " << x2 << " " << y2 << endl;
    for(int x=x1; x<x2; ++x)
        for(int y=y1; y<y2; ++y)
            if(x < xSize_ && \
               y < ySize_)
            flag_(x,y) = SOLID;
}

void StaggeredGrid::createCircle(int x, int y, int r) {
    for(int rx=-r; rx<r; ++rx)
        for(int ry=-r; ry<r; ++ry)
            if(rx*rx+ry*ry <= r &&
               x+rx < xSize_ && \
               y+ry < ySize_)
                flag_(x+rx,y+ry) = SOLID;
}

void StaggeredGrid::allgather() {
    // Send complete own section to all other ranks
    // Receive other sections from all other ranks
    
    // Building Intra-Communicator
    MPI_Comm myComm;
    MPI_Comm_split(MPI_COMM_WORLD, 0, rank_, &myComm);
    
    // p_
    MPI_Allgather(MPI_IN_PLACE, 0, MPI_DATATYPE_NULL,
                  &p_(0,0), (xSize_+2)*(ySize_+2), mpi_real,
                  MPI_COMM_WORLD);
    // rhs_
    MPI_Allgather(MPI_IN_PLACE, 0, MPI_DATATYPE_NULL,
                  &rhs_(0,0), (xSize_+2)*(ySize_+2), mpi_real,
                  MPI_COMM_WORLD);
    // u_
    MPI_Allgather(MPI_IN_PLACE, 0, MPI_DATATYPE_NULL,
                  &u_(0,0), (xSize_+2)*(ySize_+2), mpi_real,
                  MPI_COMM_WORLD);
    // v_
    MPI_Allgather(MPI_IN_PLACE, 0, MPI_DATATYPE_NULL,
                  &v_(0,0), (xSize_+2)*(ySize_+2), mpi_real,
                  MPI_COMM_WORLD);
    // f_
    MPI_Allgather(MPI_IN_PLACE, 0, MPI_DATATYPE_NULL,
                  &f_(0,0), (xSize_+2)*(ySize_+2), mpi_real,
                  MPI_COMM_WORLD);
    // g_
    MPI_Allgather(MPI_IN_PLACE, 0, MPI_DATATYPE_NULL,
                  &g_(0,0), (xSize_+2)*(ySize_+2), mpi_real,
                  MPI_COMM_WORLD);
    // flag_ <-- does not change after initialization
}
    
void StaggeredGrid::synchronizeGhostPressure() {
    // 1. Send north boundary to rank+1
    if(rank_+1 < num_procs_)
        MPI_Ssend(&p_(0, yEndOfBlock(rank_)), blockWidth(), mpi_real, 
                 rank_+1, 0, MPI_COMM_WORLD);
    // 2. Receive south boundary from rank-1
    if(rank_-1 >= 0)
        MPI_Recv(&p_(0, yEndOfBlock(rank_-1)), blockWidth(), mpi_real,
                 rank_-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    // 3. Send south boundary to to rank-1
    if(rank_-1 >= 0)
        MPI_Ssend(&p_(0, yStartOfBlock(rank_)), blockWidth(), mpi_real, 
                 rank_-1, 0, MPI_COMM_WORLD);
    // 4. Receive north boundary from rank+1
    if(rank_+1 < num_procs_)
        MPI_Recv(&p_(0, yStartOfBlock(rank_+1)), blockWidth(), mpi_real, 
                 rank_+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
}
