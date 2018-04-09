#include "classifier.hpp"
#include <stdio.h>

int main(int argc, char const *argv[]) {
  ScrewClassifier parser(argv[1]);
  parser.classify();
  return 0;
}
