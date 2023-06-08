/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_DRAW_NODE_POPUP_H_
#define VH_PONC_DRAW_NODE_POPUP_H_

#include "core_tags.h"
#include "coreui_diagram.h"
#include "draw_i_popup.h"
#include "draw_string_buffer.h"

namespace vh::ponc::draw {
///
class NodePopup : public IPopup {
 public:
  ///
  void Draw(coreui::Diagram &diagram, core::Tags &tags);

 private:
  void DrawTagActions(coreui::Diagram &diagram, core::Tags &tags,
                      const std::vector<ne::NodeId> &selected_nodes);

  ///
  StringBuffer tag_name_buffer_{};
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_NODE_POPUP_H_
