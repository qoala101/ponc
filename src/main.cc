#include "esc_app.h"
#include "esc_labels.h"

// ---
auto Main(int argc, char **argv) -> int {
  auto app = esc::App(esc::label::kApp, argc, argv);

  if (!app.Create()) {
    return 0;
  }

  return app.Run();
}