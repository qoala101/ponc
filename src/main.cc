#include <cstdlib>

#include "app_app.h"

///
auto Main(int argc, char **argv) -> int {
  auto app = vh::ponc::App("PON Calculator", argc, argv);

  if (!app.Create()) {
    return EXIT_FAILURE;
  }

  return app.Run();
}