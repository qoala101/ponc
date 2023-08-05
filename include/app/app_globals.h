#ifndef VH_PONC_APP_GLOBALS_H_
#define VH_PONC_APP_GLOBALS_H_

#include <imgui.h>

#include <memory>
#include <string>

namespace vh::ponc {
///
class AppImpl;

///
struct Globals {
  ///
  ImVec2 window_size{};
  ///
  std::string project_name{};

 private:
  ///
  friend class AppImpl;

  ///
  static auto GetInstance() -> Globals &;
};
}  // namespace vh::ponc

#endif  // VH_PONC_APP_GLOBALS_H_
