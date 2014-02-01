#include "Types.hh"
#include "FileReader.hh"
#include "StaggeredGrid.hh"
#include "Array.hh"
#include "GrayScaleImage.hh"
#include <iostream>


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
