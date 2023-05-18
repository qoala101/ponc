/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_DRAW_LINKER_H_
#define VH_PONC_DRAW_LINKER_H_

#include <vector>

#include "coreui_family.h"
#include "coreui_linker.h"
#include "draw_connect_node_popup.h"

namespace vh::ponc::draw {
///
class Linker {
 public:
  ///
  void Draw(coreui::Linker &linker,
            const std::vector<coreui::FamilyGroup> &family_groups);

 private:
  ///
  void DrawNewNodeQuery(coreui::Linker &linker, const ImVec2 &new_node_pos);

  ///
  ConnectNodePopup connect_node_popup_{};
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_LINKER_H_
