#ifndef VH_DRAW_GROUP_SETTINGS_VIEW_H_
#define VH_DRAW_GROUP_SETTINGS_VIEW_H_

#include <string>

#include "draw_i_hidable.h"

namespace esc {
class State;
}  // namespace esc

namespace esc::draw {
class GroupSettingsView : public IHidable {
 public:
  auto GetLabel() const -> std::string;
  void Draw(frame::Frame &frame);

 private:
  int group_index_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_GROUP_SETTINGS_VIEW_H_
