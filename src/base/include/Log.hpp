// Log.hpp - class for logging
//
// author: Sung-Cheol Kim @ IBM
// date: 2017/09/07 version 1.0.0 - initial version

#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <fstream>
#include "date.h"

using namespace std;

class Log {

public:
  // member functions
  void newfile(string fn);
  void timestamp(string parname, string comment);
  void write(string line);

  // Constructor
  Log(string filename, string parname) {
    ifstream f(filename.c_str());
    if (f.good()) {
      cout << "... print time stamp" << endl;
      file_.open(filename.c_str(), ios::out|ios::app);
      timestamp(parname, "[Start]");
    }
    else {
      cout << "... create new " << filename << endl;
      newfile(filename);
      timestamp(parname, "[Start]");
    }
    f.close();
  }
  Log() { Log("cloud_log.txt", "test.par"); }

  virtual ~Log() {
    timestamp("", "[End]");
    file_.close();
  }

private:
  string filename_;
  fstream file_;

};

void Log::timestamp(string parname, string comment) {
  using namespace date;
  using namespace std::chrono;

  auto l = (parname+comment).size();
  file_ << "#[" << system_clock::now() << "] " << string(50-l,'#') << " " << parname << " " << comment << endl;
}

void Log::newfile(string fn) {
  cout << "... prepare new cloud_log.txt" << endl;
  file_.open(fn.c_str(), ios::out|ios::app);
  file_ << string(80,'#') << endl
        << "# Enzyme simulation log" << endl
        << string(80,'#') << endl
        << "# 1 - Time" << endl
        << "# 2 - Total Product" << endl
        << "# 3 - Total Product Concentration [uM]" << endl
        << "# 4 - Total Product Rate [uM/s]" << endl
        << "# 6 - Wall Hit " << endl
        << "# 7 - Wall Hit Pressure [mbar]" << endl
        << "# 8 - Mean Free time [s]" << endl
        << "# 9 - Mean Free Length [um]" << endl
        << "# 10 - Count for measuring mean free time " << endl
        << string(80,'#') << endl;
}

void Log::write(string log_str) {
  file_ << log_str << endl;
}
#endif /* LOG_H */

// vim:foldmethod=syntax:foldlevel=0
