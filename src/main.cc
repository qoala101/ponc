/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include <application.h>

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