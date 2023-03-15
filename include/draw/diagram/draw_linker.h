#ifndef VH_DRAW_LINKER_H_
#define VH_DRAW_LINKER_H_

#include <vector>

#include "coreui_family.h"
#include "coreui_linker.h"
#include "draw_connect_node_popup.h"

namespace esc::draw {
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
}  // namespace esc::draw

#endif  // VH_DRAW_LINKER_H_
