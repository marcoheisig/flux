#ifndef ARRAY_HH
#define ARRAY_HH

#include <Types.hh>
#include <Debug.hh>
#include <vector>
#include <iostream>

//*******************************************************************************************************************
/*!  Array class for 1,2 and 3 dimensions
*
*    - all elements should be stored in a contiguous chunk of memory ( no vector<vector> ! )
*/
//*******************************************************************************************************************

template <typename T>
class Array {
public:
    // Constructors for 2D
    Array( int xxSize, int yySize );

    // Access Operators for 2D
    inline T & operator () ( int i ,int j );

    // for const Arrays the following access operators are required
    inline const T & operator () ( int i ,int j ) const;

    // initialize the whole array with a constant value
    void fill( T value );

    // return total size of the array
    size_t getSize() const;

    // Print the whole array ( for debugging purposes )
    void print();
    
    // Getter methods
    int xSize() const { return xSize_; }
    int ySize() const { return ySize_; }
    
    // MPI Functionality
    void allgather();
    void gather(int root=0);
    void syncGhostLayer(bool send_north=true, bool send_south=true);

    int blockHeight() { return ySize_/num_procs_; }

    int blockWidth() { return xSize_; }
    
    int myYStart() {return yStartOfBlock(rank_);}
    int myYEnd() {return yEndOfBlock(rank_);}

    int rank_;

private:
    std::vector<T> data;
    int xSize_, ySize_;
    
    int num_procs_;

    
    int yStartOfBlock(int rank) { return blockHeight()*rank; }

    int yEndOfBlock(int rank) {
        // This can be assumed, since we checked for 
        // imax+2 % num_procs_ == 0
        return blockHeight()*(rank+1)-1; }

};

//===================================================================================================================
//
//  Inline Access Operators and Sizes
//
//===================================================================================================================


// Operator() 2D
template <typename T>
inline T& Array<T>::operator ()(int i,int j)
{
    ASSERT_MSG(i<xSize_ && i>=0, "Out of bound x-coordinate");
    ASSERT_MSG(j<ySize_ && j>=0, "Out of bound y-coordinate");
    return data[(size_t)(i+j*xSize_)];
}

// Operator() 2D
template <typename T>
inline const T& Array<T>::operator ()(int i,int j) const
{
    ASSERT_MSG(i<xSize_ && i>=0, "Out of bound x-coordinate");
    ASSERT_MSG(j<ySize_ && j>=0, "Out of bound y-coordinate");
    return data[(size_t)(i+j*xSize_)];
}

#endif //ARRAY_HH

