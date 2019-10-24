#include <wfc/app.h>

#include "loguru.hpp"

int main(int argc, char *argv[]) {
  loguru::init(argc, argv);

  runConfiguruFile("samples.cfg");

  return 0;
}
