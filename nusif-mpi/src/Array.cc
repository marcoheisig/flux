#include "mpi.h"
#include "Array.hh"
#include <memory>
#include <vector>
#include <iostream>
#include <iomanip>
#include <math.h>

using namespace std;


//===================================================================================================================
//
//  Constructors
//
//===================================================================================================================

template <typename T>
Array<T>::Array( int xxSize, int yySize ) : 
    data((size_t)(xxSize*yySize)), xSize_(xxSize), ySize_(yySize)
{
    MPI_Comm_size(MPI_COMM_WORLD, &num_procs_);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank_);
}

//===================================================================================================================
//
//  Convenience Functions
//
//===================================================================================================================


//initialize the whole array with a constant value
template <typename T>
void Array<T>::fill( T value )
{
    std::fill(data.begin(), data.end(), value);
}


// Print the whole array (for debugging purposes)
template <typename T>
void Array<T>::print()
{
    int y=ySize_-1;
    cout << '[' << endl;
    do {
        for(int x=0; x<xSize_; x++) {
            cout << fixed << setw(7) << setprecision(3) << operator()(x, y) << " ";
        }
        cout << ";" << endl;
    } while(y-- != 0);
    cout << ']' << endl;
}

template <>
void Array<short>::print()
{
    int y=ySize_-1;
    cout << '[' << endl;
    do {
        for(int x=0; x<xSize_; x++) {
            cout << setw(2) << operator()(x, y) << " ";
        }
        cout << ";" << endl;
    } while(y-- != 0);
    cout << ']' << endl;
}

//return total size of the array
template <typename T>
size_t Array<T>::getSize() const
{
    return data.size();
}

template
class Array<real>;

template
class Array<short>;

template<>
void Array<real>::allgather() {
    MPI_Allgather(MPI_IN_PLACE, 0, MPI_DATATYPE_NULL,
                  &data[0], xSize_*ySize_/num_procs_, mpi_real,
                  MPI_COMM_WORLD);
}

template<>
void Array<real>::gather(int root) {
    MPI_Gather(MPI_IN_PLACE, 0, MPI_DATATYPE_NULL,
               &data[0], xSize_*ySize_/num_procs_, mpi_real,
               root, MPI_COMM_WORLD);
}

template<>
void Array<real>::syncGhostLayer(bool send_north, bool send_south) {
    if(send_north) {
        // 1. Send north boundary to rank+1
        if(rank_+1 < num_procs_)
            MPI_Ssend(&(*this)(0, yEndOfBlock(rank_)), blockWidth(), mpi_real, 
                     rank_+1, 1, MPI_COMM_WORLD);
        // 2. Receive south boundary from rank-1
        if(rank_-1 >= 0)
            MPI_Recv(&(*this)(0, yEndOfBlock(rank_-1)), blockWidth(), mpi_real,
                     rank_-1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
    
    if(send_south) {
        // 3. Send south boundary to to rank-1
        if(rank_-1 >= 0)
            MPI_Ssend(&(*this)(0, yStartOfBlock(rank_)), blockWidth(), mpi_real, 
                     rank_-1, 0, MPI_COMM_WORLD);
        // 4. Receive north boundary from rank+1
        if(rank_+1 < num_procs_)
            MPI_Recv(&(*this)(0, yStartOfBlock(rank_+1)), blockWidth(), mpi_real, 
                     rank_+1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
}
