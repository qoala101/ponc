#include "esc_app.h"

auto Main(int argc, char **argv) -> int {
  auto example = App{"Blueprints", argc, argv};

  if (!example.Create()) {
    return 0;
  }

  return example.Run();
}