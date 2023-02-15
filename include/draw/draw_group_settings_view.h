#ifndef VH_DRAW_GROUP_SETTINGS_VIEW_H_
#define VH_DRAW_GROUP_SETTINGS_VIEW_H_

#include "draw_i_drawable.h"

namespace esc::draw {
class GroupSettingsView : public IDrawable {
 public:
  void Draw(State &state) override;

 private:
  int selected_group_index_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_GROUP_SETTINGS_VIEW_H_
