﻿#include "esc_example.h"

auto main(int argc, char **argv) -> int {
  auto example = Example{"Blueprints", argc, argv};

  if (!example.Create()) {
    return 0;
  }

  return example.Run();
}