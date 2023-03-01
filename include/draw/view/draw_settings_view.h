#ifndef VH_DRAW_SETTINGS_VIEW_H_
#define VH_DRAW_SETTINGS_VIEW_H_

#include <string>

#include "core_settings.h"
#include "coreui_project.h"
#include "draw_i_hidable.h"
#include "imgui.h"

namespace esc::draw {
class SettingsView : public IHidable {
 public:
  auto GetLabel() const -> std::string;

  void Draw(core::Settings &settings);
};
}  // namespace esc::draw

#endif  // VH_DRAW_SETTINGS_VIEW_H_
