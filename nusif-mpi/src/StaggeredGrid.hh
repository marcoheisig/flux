#ifndef STAGGERED_GRID_HH
#define STAGGERED_GRID_HH


#include "Types.hh"
#include "Array.hh"
#include "FileReader.hh"

// Possible cell type:
typedef enum {FLUID, SOLID} Celltype;

// Possible 2D directions:
typedef enum {NORTH, EAST, SOUTH, WEST} Direction;

//*******************************************************************************************************************
/*! Class for storing all arrays required for the simulation
*
* For now it only contains an array for the pressure and another array for the
* right hand side of the pressure equation.
* In following assignments this will be extended and also contain
* arrays for x/y velocity and some arrays for intermediate values.
*
* Feel free to add member functions or variables to this class, but please don't
* delete or rename any existing functions, since future skeletons rely on these functions.
*
*/
//*******************************************************************************************************************
class StaggeredGrid
{
public:
    // Constructors to manually create staggered grid
    StaggeredGrid ( int xxSize, int yySize, real ddx, real ddy );

    // Constructor to create a staggered grid from a parsed configuration file
    StaggeredGrid ( const FileReader & configuration );


    // Getters / Setters for member variables
    Array<real> & p()      { return p_;    }
    Array<real> & rhs()    { return rhs_;  }
    Array<real> & u()      { return u_;    }
    Array<real> & v()      { return v_;    }
    Array<real> & f()      { return f_;    }
    Array<real> & g()      { return g_;    }
    Array<short> & flag()  { return flag_; }

    const Array<real>  & p()    const { return p_;    }
    const Array<real>  & rhs()  const { return rhs_;  }
    const Array<real>  & u()    const { return u_;    }
    const Array<real>  & v()    const { return v_;    }
    const Array<real>  & f()    const { return f_;    }
    const Array<real>  & g()    const { return g_;    }
    const Array<short> & flag() const { return flag_; }
    
    inline bool isFluid(const int x, const int y) {return flag()(x,y) == FLUID;}
    
    // wrapped access
    inline real u(const int x, const int y) {return u_(x,y);}
    inline real v(const int x, const int y) {return v_(x,y);}
    inline real p(const int x, const int y) {return p_(x,y);}
    inline real u(const int x, const int y, Direction dir);
    inline real v(const int x, const int y, Direction dir);
    inline real p(const int x, const int y, Direction dir);

    real dx() const { return dx_; }
    real dy() const { return dy_; }
    
    int xSize() const { return xSize_; }
    int ySize() const { return ySize_; }

    int rank() { return p_.rank_; };
    
    void createRectangle(int x1, int y1, int x2, int y2);
    void createCircle   (int x, int y, int r);


protected:
    int xSize_;
    int ySize_;

    
    Array<real> p_;      //< pressure field
    Array<real> rhs_;    //< right hand side of the pressure equation
    Array<real> u_;      //< first velocity component
    Array<real> v_;      //< second velocity component
    Array<real> f_;      //< first momentum(?) component
    Array<real> g_;      //< second momentum(?) component
    Array<short> flag_;  //< second momentum(?) component

    real dx_;   //< distance between two grid points in x direction
    real dy_;   //< distance between two grid points in y direction
};

inline real StaggeredGrid::u(const int x, const int y, Direction dir) {
    int nx = x;
    int ny = y;
    
    // New coordinate:
    if(dir == NORTH)
        ny += 1;
    else if(dir == SOUTH)
        ny -= 1;
    else if(dir == WEST)
        nx -= 1;
    else
        nx += 1;

    if(isFluid(nx, ny)) {
        // FLUID
        return u(nx, ny);
    } else {
        // OBSTACLE with no-slip boundary condition
        return -u(x, y);
    }
}

inline real StaggeredGrid::v(const int x, const int y, Direction dir) {
    int nx = x;
    int ny = y;
    
    // New coordinate:
    if(dir == NORTH)
        ny += 1;
    else if(dir == SOUTH)
        ny -= 1;
    else if(dir == WEST)
        nx -= 1;
    else
        nx += 1;

    if(isFluid(nx, ny)) {
        // FLUID
        return v(nx, ny);
    } else {
        // OBSTACLE with no-slip boundary condition
        return -v(x, y);
    }
}

inline real StaggeredGrid::p(const int x, const int y, Direction dir) {
    int nx = x;
    int ny = y;
    
    // New coordinate:
    if(dir == NORTH)
        ny += 1;
    else if(dir == SOUTH)
        ny -= 1;
    else if(dir == WEST)
        nx -= 1;
    else
        nx += 1;
    
    if(isFluid(nx, ny)) {
        // FLUID
        return p(nx, ny);
    } else {
        // OBSTACLE with neumann boundary condition
        return p(x, y);
    }
}

#endif //STAGGERED_GRID_HH

