#ifndef VH_DRAW_BACKGROUND_POPUP_H_
#define VH_DRAW_BACKGROUND_POPUP_H_

#include "draw_i_popup.h"
#include "imgui.h"

namespace esc::draw {
// ---
class BackgroundPopup : public IPopup {
 public:
  // ---
  explicit BackgroundPopup(const ImVec2 &position);

 private:
  // ---
  auto GetLabel [[nodiscard]] (State &state) const -> std::string override;
  // ---
  void DrawContent(State &state) override;

  // ---
  ImVec2 position_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_BACKGROUND_POPUP_H_
