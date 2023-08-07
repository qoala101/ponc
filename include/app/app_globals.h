/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_APP_GLOBALS_H_
#define VH_PONC_APP_GLOBALS_H_

#include <imgui.h>
#include <imgui_internal.h>

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

  Globals() = default;

  ///
  static auto GetInstance() -> Globals&;
  ///
  static void OnStart();
  ///
  static void ReadLineHandler(ImGuiContext* /*unused*/,
                              ImGuiSettingsHandler* /*unused*/,
                              void* /*unused*/, const char* line);
  ///
  static void WriteAllHandler(ImGuiContext* /*unused*/,
                              ImGuiSettingsHandler* handler,
                              ImGuiTextBuffer* buffer);
};
}  // namespace vh::ponc

#endif  // VH_PONC_APP_GLOBALS_H_
