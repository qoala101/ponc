#ifndef VH_DRAW_DIAGRAM_EDITOR_H_
#define VH_DRAW_DIAGRAM_EDITOR_H_

#include <imgui_node_editor.h>

#include <memory>

#include "core_diagram.h"
#include "coreui_diagram.h"
#include "draw_create_node_popup.h"
#include "draw_link_popup.h"
#include "draw_linking.h"
#include "draw_node_popup.h"
#include "draw_nodes.h"

namespace esc::draw {
///
class DiagramEditor {
 public:
  ///
  void Draw(coreui::Diagram &diagram);

 private:
  ///
  void OpenPopupsIfRequested(const core::Diagram &diagram);
  ///
  void DrawPopups(coreui::Diagram &diagram);
  ///
  void DrawCreateNodePopup(coreui::Diagram &diagram);

  ///
  Linking linking_{};
  ///
  Nodes nodes_{};
  ///
  CreateNodePopup create_node_popup_{};
  ///
  NodePopup node_popup_{};
  ///
  LinkPopup link_popup_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_DIAGRAM_EDITOR_H_
