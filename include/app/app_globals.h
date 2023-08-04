#ifndef VH_PONC_APP_GLOBALS_H_
#define VH_PONC_APP_GLOBALS_H_

#include <imgui.h>

#include <string>

namespace vh::ponc {
///
struct Globals {
  ///
  Globals();

  ///
  ImVec2 window_size{};
  ///
  std::string project_name{};
};
}  // namespace vh::ponc

#endif  // VH_PONC_APP_GLOBALS_H_
