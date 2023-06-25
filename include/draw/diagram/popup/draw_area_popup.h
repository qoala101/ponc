/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_DRAW_AREA_POPUP_H_
#define VH_PONC_DRAW_AREA_POPUP_H_

#include <imgui_node_editor.h>

#include "coreui_diagram.h"

namespace vh::ponc::draw {
///
class AreaPopup {
 public:
  ///
  void Draw(const std::vector<ne::NodeId> &selected_areas,
            coreui::Diagram &diagram);
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_AREA_POPUP_H_
