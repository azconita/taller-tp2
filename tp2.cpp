#include <stdio.h>
#include "packager.h"

int main(int argc, char const *argv[]) {
  ScrewPackager packager(argv[1]);
  packager.connect_to_classificators(argc -2, &argv[2]);
  packager.start();
  return 0;
}
