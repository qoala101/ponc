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
#include <unordered_map>
#include <variant>

#include "cpp_assert.h"
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
class Globals {
 public:
  ///
  template <typename T>
  void Set(const std::string& name, T value) {
    settings_[name] = std::move(value);
  }

  ///
  template <typename T>
  auto GetOr(const std::string& name, const T& default_value) const
      -> const T& {
    const auto setting = settings_.find(name);

    if (setting == settings_.cend()) {
      return default_value;
    }

    Expects(std::holds_alternative<T>(setting->second));
    return std::get<T>(setting->second);
  }

 private:
  ///
  using Setting = std::variant<std::string, bool>;
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
  std::unordered_map<std::string, Setting> settings_{};
};
}  // namespace vh::ponc

#endif  // VH_PONC_APP_GLOBALS_H_
