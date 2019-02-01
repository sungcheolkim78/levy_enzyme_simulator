// simpleWalker.cpp
//
// author: sungcheolkim @ IBM
// date: 20160201 version: 1.0.0 - base class for walker
// date: 20160616 version: 1.1.0 - using template on cloud
// date: 2017/09/07 version: 1.2.0 - using updated library

#include "../base/include/Simulator.hpp"
#include "../base/include/Log.hpp"
#include "../base/include/ParameterReader.h"
#include "../base/include/Cloud.hpp"
//#include "../base/include/SurfacesBox.hpp"
//#include "../base/include/SurfacesSphere.hpp"
#include "../base/include/SurfacesCell.hpp"

int main(int argc, char* argv[])
{
    string parname {"test.par"};
    cout << "[simpleWalker] random walker simulator, made by Sung-Cheol Kim, version 1.2.0" << endl;

    if (argc == 2)
      parname = argv[1];

    ifstream f(parname.c_str());
    if ( !f.good() ) {
      cerr << "... no " << parname << endl;
      cerr << "Usage: simpleWalker [sim.par]" << endl;
      exit(0);
    }

    // read parameter file
    Log simLog{"cloud_log.txt", parname};
    ParameterReader pr{parname};

    // create cell cloud object
    Simulator<Walker, SurfacesCell> s{pr};
    Cloud<Walker, SurfacesCell> wc1{pr, "Walker1"};
    Cloud<Walker, SurfacesCell> wc2{pr, "Walker2"};

    wc1.setRnd(s.getRnd());
    wc2.setRnd(s.getRnd());

    wc1.injectWalkers(pr);
    wc2.injectWalkers(pr);

    s.addCloud(wc1);
    s.addCloud(wc2);

    // iterates for time steps
    cout << "[simpleWalker] start simulation" << endl;
    s.run();
    // show final results
    //s.info();
}
