// app_cloud.cpp
// application using cloud class
//
// author: sungcheolkim @ IBM
// date: 20160202
//

#include "../base/include/Cloud.hpp"
#include "../base/include/LevyWalker.h"
#include <iostream>

class ParameterReader;

int main(int argc, char** argv)
{
    string parname = "sim.par";

    if (argc == 2)
        parname = argv[1];

    ifstream f(parname.c_str());
    if ( f.good() )
        cout << "... read parameters from " << parname << endl;
    else {
        cerr << "... no " << parname << endl;
        cerr << "Usage: levyWalker [sim.par]" << endl;
        exit(1);
    }

    ParameterReader pr(parname);
    Cloud<LevyWalker> c("pt.temp");

    // insert walkers
    c.injectWalker(pr);
    int iteration_ = pr.iteration();
    double dt_ = pr.dt();

    // iterates for time steps
    for(int i=0; i<iteration_; i++)
        c.moveWalker(dt_);

    cout << "... Run for " << pr.dt()*pr.iteration() << " secs" << endl;
    // c.info();
}
