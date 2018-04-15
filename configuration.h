#ifndef SCREW_CONFIGURATION_H
#define SCREW_CONFIGURATION_H

#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <utility>

class ScrewConfiguration {
private:
  std::map<int,std::pair<std::string,int>> screw_types;
public:
  ScrewConfiguration (const char* cfg_file);
  std::vector<int> get_types();
  std::string get_type(int ntype);
  bool has_type(int type);
  int get_limit(int ntype);
  virtual ~ScrewConfiguration ();
};


#endif
