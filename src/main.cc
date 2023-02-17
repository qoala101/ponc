/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#include "esc_app.h"

// ---
auto Main(int argc, char **argv) -> int {
  auto app = esc::App("PON Calculator", argc, argv);

  if (!app.Create()) {
    return 0;
  }

  return app.Run();
}