/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_APP_GLOBALS_H_
#define VH_PONC_APP_GLOBALS_H_

#include <crude_json.h>
#include <imgui.h>
#include <imgui_internal.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <variant>

#include "cpp_static_api.h"

namespace vh::ponc {
///
class Globals;

///
struct GlobalsProxy : public cpp::StaticApi {
  ///
  static auto GetInstance() -> Globals&;
  ///
  static void RegisterHandlers();
};

///
using Global = std::variant<std::string, bool>;

///
class Globals {
 public:
  ///
  template <typename T>
  void Set(const std::string& name, const T& value,
           const T& default_value = {}) {
    if (value == default_value) {
      settings_json_.erase(name);
      return;
    }

    settings_json_[name] = value;
  }

  ///
  template <typename T>
  auto GetOr(const std::string& name, const T& default_value) const
      -> const T& {
    if (!settings_json_.contains(name)) {
      return default_value;
    }

    const auto* value = settings_json_[name].get_ptr<T>();

    if (value == nullptr) {
      return default_value;
    }

    return *value;
  }

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

  ///
  crude_json::value settings_json_{};
};
}  // namespace vh::ponc

#endif  // VH_PONC_APP_GLOBALS_H_
