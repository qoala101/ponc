/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#ifndef VH_DRAW_BACKGROUND_POPUP_H_
#define VH_DRAW_BACKGROUND_POPUP_H_

#include <imgui.h>

#include "draw_i_popup.h"
#include "frame_node.h"

namespace esc::draw {
class BackgroundPopup : public IPopup {
 public:
  void SetPosition(const ImVec2 &position);

 private:
  auto GetLabel() const -> std::string override;
  void DrawItems(frame::Frame &frame) override;

  ImVec2 position_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_BACKGROUND_POPUP_H_
