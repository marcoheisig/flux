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

private:
    std::vector<T> data;
    int xSize_, ySize_;

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

