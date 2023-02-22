#ifndef VH_DRAW_SETTINGS_VIEW_H_
#define VH_DRAW_SETTINGS_VIEW_H_

#include <string>

#include "app_state.h"
#include "core_settings.h"
#include "draw_i_hidable.h"
#include "imgui.h"

namespace esc::draw {
class SettingsView : public IHidable {
 public:
  auto GetLabel [[nodiscard]] () const -> std::string;

  void Draw(const AppState &app_state);

  auto GetColorForFlowValue
      [[nodiscard]] (float value, const core::Settings &settings) const
      -> ImColor;

 private:
  bool color_flow{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_SETTINGS_VIEW_H_
