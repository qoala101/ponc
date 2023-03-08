#ifndef VH_DRAW_DIAGRAM_EDITOR_H_
#define VH_DRAW_DIAGRAM_EDITOR_H_

#include <imgui_node_editor.h>

#include <memory>

#include "coreui_diagram.h"
#include "draw_create_node_popup.h"
#include "draw_linking.h"
#include "draw_nodes.h"

namespace ne = ax::NodeEditor;

namespace esc::draw {
///
class DiagramEditor {
 public:
  ///
  DiagramEditor();

  ///
  void Draw(coreui::Diagram &diagram);

 private:
  ///
  void DrawPopups(coreui::Diagram &diagram);
  ///
  void DrawCreateNodePopup(coreui::Diagram &diagram);

  ///
  std::unique_ptr<ne::EditorContext, void (*)(ne::EditorContext *)> context_;
  ///
  Linking linking_{};
  ///
  Nodes nodes_{};
  ///
  CreateNodePopup create_node_popup_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_DIAGRAM_EDITOR_H_
