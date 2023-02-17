#ifndef VH_DRAW_BACKGROUND_POPUP_H_
#define VH_DRAW_BACKGROUND_POPUP_H_

#include <imgui.h>

#include "draw_i_popup.h"

namespace esc::draw {
// ---
class BackgroundPopup : public IPopup {
 public:
  void SetPosition(const ImVec2 &position);

 private:
  // ---
  auto GetLabel [[nodiscard]] () const -> std::string override;
  // ---
  void DrawContent(State &state) override;

  // ---
  ImVec2 position_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_BACKGROUND_POPUP_H_
