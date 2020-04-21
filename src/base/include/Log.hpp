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
  void timestamp(string comment);
  void write(string line);

  // Constructor
  Log(string filename, string parname) {
    ifstream f(filename.c_str());
    if (f.good()) {
      cout << "... print time stamp" << endl;
      file_.open(filename.c_str(), ios::out|ios::app);
    }
    else {
      cout << "... create new " << filename << endl;
      newfile(filename);
    }
    f.close();
  }
  Log() { Log("cloud_log.txt", "test.par"); }

  virtual ~Log() {
    timestamp("[End]");
    file_.close();
  }

private:
  string filename_;
  fstream file_;
  
};

void Log::timestamp(string comment) {
  using namespace date;
  using namespace std::chrono;

  auto l = comment.size();
  file_ << "# [" << system_clock::now() << "] " << comment << " " << string(58-l, '-') << endl;
  file_ << '#' << string(89,'-') << endl;
}

void Log::newfile(string fn) {
  cout << "... prepare new cloud_log.txt" << endl;
  file_.open(fn.c_str(), ios::out|ios::app);
  file_ << '#' << string(89,'-') << endl
        << "# Enzyme simulation log" << endl
        << '#' << string(89,'-') << endl
        << "# Column Information: " << endl
        << "# 1 - Time" << endl
        << "# 2 - Cluster concentration [uM]" << endl
        << "# 3 - Cluster radius [nm]" << endl
        << "# 4 - Substrate concentration [uM]" << endl
        << "# 5 - Total Product" << endl
        << "# 6 - Total Product Concentration [uM]" << endl
        << "# 7 - Total Product Rate [uM/s]" << endl
        << "# 8 - Wall Hit " << endl
        << "# 9 - Wall Hit Pressure [mbar]" << endl
        << "# 10 - Mean Free time [s]" << endl
        << "# 11 - Mean Free Length [um]" << endl
        << "# 12 - Count for measuring mean free time " << endl
        << "# 13 - Simulation ID" << endl
        << "#" << string(89,'-') << endl;
}

void Log::write(string log_str) {
  file_ << log_str << endl;
}
#endif /* LOG_H */

// vim:foldmethod=syntax:foldlevel=0
