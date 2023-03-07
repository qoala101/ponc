#ifndef VH_DRAW_LINK_CREATION_H_
#define VH_DRAW_LINK_CREATION_H_

#include <imgui_node_editor.h>

#include <optional>
#include <string>

#include "coreui_link_creation.h"
#include "draw_create_node_popup.h"
#include "imgui.h"

namespace esc::draw {
///
class LinkCreation {
 public:
  ///
  struct Callbacks {
    ///
    cpp::Query<ImVec2, ne::PinId> get_pin_tip_pos{};
  };

  void Draw(coreui::LinkCreation &link_creation,
            const std::vector<coreui::FamilyGroup> &family_groups,
            const Callbacks &callbacks);

 private:
  void DrawNewNodeQuery(coreui::LinkCreation &link_creation,
                        const ImVec2 &new_node_pos);

  auto GetPos(const coreui::PosVariant &pos_variant,
              const Callbacks &callbacks) const;

  ///
  CreateNodePopup create_node_popup_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_LINK_CREATION_H_
