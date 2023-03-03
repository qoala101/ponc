#ifndef VH_DRAW_BACKGROUND_POPUP_H_
#define VH_DRAW_BACKGROUND_POPUP_H_

#include <imgui.h>

#include <memory>

#include "coreui_family.h"
#include "draw_i_popup.h"

namespace esc::draw {
///
class BackgroundPopup : public IPopup {
 public:
  ///
  void Draw(const std::vector<coreui::FamilyGroup> &family_groups);
  ///
  void SetPos(const ImVec2 &pos);

 private:
  ///
  auto GetLabel() const -> std::string override;

  ///
  ImVec2 pos_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_BACKGROUND_POPUP_H_
