/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_DRAW_BACKGROUND_POPUP_H_
#define VH_PONC_DRAW_BACKGROUND_POPUP_H_

#include <imgui.h>

#include "coreui_diagram.h"
#include "draw_i_popup.h"
#include "draw_string_buffer.h"

namespace vh::ponc::draw {
///
class BackgroundPopup : public IPopup {
 public:
  ///
  void Draw(coreui::Diagram &diagram);
  ///
  void SetPos(const ImVec2 &pos);

 private:
  auto DrawRenamePopup();

  ///
  ImVec2 pos_{};

  StringBuffer area_name_buffer_{};
  bool focus_name_input_{};
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_BACKGROUND_POPUP_H_
