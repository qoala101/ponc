#ifndef VH_DRAW_LINK_CREATION_H_
#define VH_DRAW_LINK_CREATION_H_

#include <imgui_node_editor.h>

#include <optional>
#include <string>

#include "coreui_link_creation.h"
#include "cpp_callbacks.h"
#include "imgui.h"

namespace esc::draw {
///
class LinkCreation {
 public:
  ///
  struct Callbacks {
    ///
    cpp::Query<ImVec2, ne::PinId> get_pin_tip_pos{};
    ///
    cpp::Signal<ImVec2> new_node_requested_at{};
  };

  ///
  void Draw(coreui::LinkCreation &link_creation, const Callbacks &callbacks);

 private:
  ///
  void DrawNewNodeQuery(coreui::LinkCreation &link_creation,
                        const Callbacks &callbacks) const;
  ///
  auto GetPosValue(const coreui::PosVariant &pos_variant,
                   const Callbacks &callbacks) const;
  ///
  void DrawManualLink(const coreui::ManualLink &link,
                      const Callbacks &callbacks) const;

  ///
  ImVec2 new_node_pos_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_LINK_CREATION_H_
