#ifndef PARSER_PACKAGER_H
#define PARSER_PACKAGER_H

#include <stdio.h>
#include <string.h>
#include <fstream>
#include <iostream>
#include <queue>
#include "classifier.hpp"

class ScrewPackager {
private:
  

public:
  ScrewPackager (const char * cfg_file);
  virtual ~ScrewPackager ();
};

#endif
