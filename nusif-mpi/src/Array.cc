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
