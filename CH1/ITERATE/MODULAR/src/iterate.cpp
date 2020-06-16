#include "iterate.h"
int main(int argc, char ** argv) {
  Control *control = initialize(argc, argv);
  execute(control);
  finalize(control);
  return 0;
}
