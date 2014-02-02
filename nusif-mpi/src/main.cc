#define _USE_MATH_DEFINES

#include "mpi.h"

#include "Array.hh"
#include "FileReader.hh"
#include "Debug.hh"
#include "FluidSimulator.hh"

#include <iostream>
#include <sys/utsname.h>

using namespace std;

int main( int argc, char** argv )
{
    if(argc != 2) {
        cout << "Usage: " << argv[0] << " configfile" << endl;
        exit(1);
    }
    
    int ierr, rank, num_procs;
    ierr = MPI_Init ( &argc, &argv );
    ierr = MPI_Comm_rank ( MPI_COMM_WORLD, &rank );
    ierr = MPI_Comm_size ( MPI_COMM_WORLD, &num_procs );
    
    struct utsname unameData;
    uname(&unameData);
    
    cout << "[" << rank << "] Hello from rank "<< rank <<" on "<< unameData.nodename << ", out of " << num_procs << " ranks."<< endl;
    
    FileReader cfg;
    cfg.registerStringParameter("name");
    
    cfg.registerRealParameter("GX");
    cfg.registerRealParameter("GY");
    cfg.registerRealParameter("U_INIT");
    cfg.registerRealParameter("V_INIT");
    cfg.registerRealParameter("P_INIT");
    
    cfg.registerRealParameter("xlength");
    cfg.registerRealParameter("ylength");
    cfg.registerIntParameter("imax");
    cfg.registerIntParameter("jmax");
    
    cfg.registerRealParameter("dt");
    cfg.registerIntParameter("timesteps");
    cfg.registerRealParameter("Re");
    cfg.registerRealParameter("safetyfactor");
    
    cfg.registerIntParameter("itermax");
    cfg.registerRealParameter("eps");
    cfg.registerRealParameter("omg");
    cfg.registerRealParameter("gamma");
    cfg.registerIntParameter("checkfrequency", 1);
    cfg.registerIntParameter("normalizationfrequency", 10);
    
    cfg.registerIntParameter("outputinterval", 1);
    
    cfg.registerRealParameter("boundary_velocity_N", 0.0);
    cfg.registerRealParameter("boundary_velocity_S", 0.0);
    cfg.registerRealParameter("boundary_velocity_W", 0.0);
    cfg.registerRealParameter("boundary_velocity_E", 0.0);
    
    cfg.registerStringParameter("boundary_condition_N", "noslip");
    cfg.registerStringParameter("boundary_condition_S", "noslip");
    cfg.registerStringParameter("boundary_condition_W", "noslip");
    cfg.registerStringParameter("boundary_condition_E", "noslip");
    
    cfg.registerRealParameter("RectangleX1", -1.0);
    cfg.registerRealParameter("RectangleX2", -1.0);
    cfg.registerRealParameter("RectangleY1", -1.0);
    cfg.registerRealParameter("RectangleY2", -1.0);
    
    cfg.registerRealParameter("CircleX", -1.0);
    cfg.registerRealParameter("CircleY", -1.0);
    cfg.registerRealParameter("CircleR", -1.0);
    
    cfg.registerStringParameter("ObstacleFile", "");
    cfg.registerIntParameter("threshold", 1);
    
    // MPI parameters:
    cfg.registerIntParameter("rank", rank);
    cfg.registerIntParameter("num_procs", num_procs);
    
    bool res = cfg.readFile(argv[1]);
    CHECK_MSG(res, "Could not open config file.");
    
    // Checking boundary condition and velocity sanity
    CHECK_MSG(cfg.getStringParameter("boundary_condition_N") == "noslip" || 
              cfg.getStringParameter("boundary_condition_N") == "outflow" || 
              cfg.getStringParameter("boundary_condition_N") == "inflow",
              "Invalid north boundary condition.");
    CHECK_MSG(cfg.getStringParameter("boundary_condition_S") == "noslip" || 
              cfg.getStringParameter("boundary_condition_S") == "outflow" || 
              cfg.getStringParameter("boundary_condition_S") == "inflow",
              "Invalid north boundary condition.");
    CHECK_MSG(cfg.getStringParameter("boundary_condition_W") == "noslip" || 
              cfg.getStringParameter("boundary_condition_W") == "outflow" || 
              cfg.getStringParameter("boundary_condition_W") == "inflow",
              "Invalid north boundary condition.");
    CHECK_MSG(cfg.getStringParameter("boundary_condition_E") == "noslip" || 
              cfg.getStringParameter("boundary_condition_E") == "outflow" || 
              cfg.getStringParameter("boundary_condition_E") == "inflow",
              "Invalid north boundary condition.");
    CHECK_MSG(!(cfg.getStringParameter("boundary_condition_N") == "outflow" && 
                cfg.getRealParameter("boundary_velocity_N") != 0.0),
              "North boundary condition and velocity does not make sense.");
    CHECK_MSG(!(cfg.getStringParameter("boundary_condition_S") == "outflow" && 
                cfg.getRealParameter("boundary_velocity_S") != 0.0),
              "South boundary condition and velocity does not make sense.");
    CHECK_MSG(!(cfg.getStringParameter("boundary_condition_W") == "outflow" && 
                cfg.getRealParameter("boundary_velocity_W") != 0.0),
              "West boundary condition and velocity does not make sense.");
    CHECK_MSG(!(cfg.getStringParameter("boundary_condition_E") == "outflow" &&
                cfg.getRealParameter("boundary_velocity_E") != 0.0),
              "East boundary condition and velocity does not make sense.");
    CHECK_MSG(!(cfg.getStringParameter("boundary_condition_N") == "inflow" && 
                cfg.getRealParameter("boundary_velocity_N") == 0.0),
              "North boundary condition and velocity does not make sense.");
    CHECK_MSG(!(cfg.getStringParameter("boundary_condition_S") == "inflow" &&
                cfg.getRealParameter("boundary_velocity_S") == 0.0),
              "South boundary condition and velocity does not make sense.");
    CHECK_MSG(!(cfg.getStringParameter("boundary_condition_W") == "inflow" &&
                cfg.getRealParameter("boundary_velocity_W") == 0.0),
              "West boundary condition and velocity does not make sense.");
    CHECK_MSG(!(cfg.getStringParameter("boundary_condition_E") == "inflow" &&
                cfg.getRealParameter("boundary_velocity_E") == 0.0),
              "East boundary condition and velocity does not make sense.");
    
    CHECK_MSG(cfg.getRealParameter("RectangleX1") < cfg.getRealParameter("xlength"),
              "Out of bound rectangle coordinate.");
    CHECK_MSG(cfg.getRealParameter("RectangleX2") < cfg.getRealParameter("xlength"), 
              "Out of bound rectangle coordinate.");
    CHECK_MSG(cfg.getRealParameter("RectangleY1") < cfg.getRealParameter("ylength"),
              "Out of bound rectangle coordinate.");
    CHECK_MSG(cfg.getRealParameter("RectangleY2") < cfg.getRealParameter("ylength"),
              "Out of bound rectangle coordinate.");
    CHECK_MSG((cfg.getRealParameter("CircleX")+cfg.getRealParameter("CircleR")) < cfg.getRealParameter("xlength"),
              "Out of bound circle.");
    CHECK_MSG((cfg.getRealParameter("CircleY")+cfg.getRealParameter("CircleR")) < cfg.getRealParameter("ylength"),
              "Out of bound circle.");
    
    CHECK_MSG(cfg.getRealParameter("RectangleX1") <= cfg.getRealParameter("RectangleX2"),
              "Wrong rectangle point order.");
    CHECK_MSG(cfg.getRealParameter("RectangleY1") <= cfg.getRealParameter("RectangleY2"),
              "Wrong rectangle point order.");
    
    // Checking for imax+2 % num_procs == 0
    //          and jmax+2 % num_procs == 0
    //CHECK_MSG((cfg.getIntParameter("imax")+2)%num_procs == 0, "imax+2 has to be dividable by the number of MPI processes! ")
    CHECK_MSG((cfg.getIntParameter("jmax")+2)%num_procs == 0, "jmax+2 has to be dividable by the number of MPI processes! ")
    
    
    FluidSimulator sim(cfg);
    sim.simulateTimeStepCount(cfg.getIntParameter("timesteps"));
    
    cout << "["<<rank<<"]" << "done" << endl;
    
    MPI_Finalize();
    
    return 0;
}
