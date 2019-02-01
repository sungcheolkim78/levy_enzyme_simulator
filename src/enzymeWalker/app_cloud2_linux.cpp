// app_cloud2.cpp
// application using cloud class
//
// author: sungcheolkim @ IBM
// date: 20160202
//

#include "CloudInContainer.h"
#include <iostream>

class ParameterReader;
using namespace std;

int main(int argc, char* argv[])
{
    string parname = "sim.par";

    if (argc == 2)
        parname = argv[1];
    else
        cout << "... read parameters from " << parname << endl;

    ParameterReader pr(parname);

    // create cloud object
    CloudInContainer c("pt.temp");

    // set container and active sites
    c.setProperties(pr);

    c.buildActiveSite();

    // insert walkers
    c.injectWalker(pr);

    // iterates for time steps
    for(int i=0; i<pr.iteration(); i++)
        c.runRandom(pr.dt());

    pr.info();
    cout << "... Run for " << pr.dt()*pr.iteration() << " secs" << endl;
    cout << "... Active site visits :" << c.TotalActive() << endl;
    cout << "... Active site efficiency [Hz] :" << c.ActiveEfficiency() << endl;
    cout << "... Active Mean free path [um] :" << c.ActiveMFPath() << endl;
    cout << "... Wall site visits :" << c.TotalWall() << endl;
    cout << "... Wall site efficiency [Hz] :" << c.WallEfficiency() << endl;
    cout << "... Wall Mean free path [um] :" << c.WallMFPath() << endl;

}
