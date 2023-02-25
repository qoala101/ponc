#ifndef VH_DRAW_SETTINGS_VIEW_H_
#define VH_DRAW_SETTINGS_VIEW_H_

#include <string>

#include "core_settings.h"
#include "draw_i_hidable.h"
#include "frame_node.h"
#include "imgui.h"

namespace esc::draw {
class SettingsView : public IHidable {
 public:
  auto GetLabel() const -> std::string;

  void Draw(frame::Frame &frame);
};
}  // namespace esc::draw

#endif  // VH_DRAW_SETTINGS_VIEW_H_
