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

#include "cpp_static_api.h"

namespace vh::ponc {
///
struct Globals;

///
struct GlobalsProxy : public cpp::StaticApi {
  ///
  static auto GetInstance() -> Globals&;
  ///
  static void RegisterHandlers();
};

///
struct Globals {
  ///
  ImVec2 window_size{};
  ///
  std::string project_name{};

 private:
  ///
  friend struct GlobalsProxy;

  ///
  Globals() = default;

  ///
  static auto GetInstance() -> Globals&;
  ///
  static void RegisterHandlers();
  ///
  static void OnReadLine(ImGuiContext* /*unused*/,
                         ImGuiSettingsHandler* /*unused*/, void* /*unused*/,
                         const char* line);
  ///
  static void OnWriteAll(ImGuiContext* /*unused*/,
                         ImGuiSettingsHandler* handler,
                         ImGuiTextBuffer* buffer);
};
}  // namespace vh::ponc

#endif  // VH_PONC_APP_GLOBALS_H_
