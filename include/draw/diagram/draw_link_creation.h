#ifndef VH_DRAW_LINK_CREATION_H_
#define VH_DRAW_LINK_CREATION_H_

#include <imgui_node_editor.h>

#include <optional>
#include <string>

#include "coreui_project.h"
#include "draw_background_connection_popup.h"
#include "imgui.h"

namespace esc::draw {
///
class LinkCreation {
 public:
  ///
  void Draw(coreui::LinkCreation &link_creation,
            const std::vector<coreui::FamilyGroup> &family_groups);

 private:
  ///
  void DrawNewNodeQuery(coreui::LinkCreation &link_creation,
                        const ImVec2 &new_node_pos);

  ///
  BackgroundConnectionPopup connection_popup_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_LINK_CREATION_H_
