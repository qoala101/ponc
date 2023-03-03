#ifndef VH_DRAW_DIAGRAM_EDITOR_H_
#define VH_DRAW_DIAGRAM_EDITOR_H_

#include <imgui_node_editor.h>

#include <memory>

#include "coreui_diagram.h"

namespace ne = ax::NodeEditor;

namespace esc::draw {
///
class DiagramEditor {
 public:
  ///
  DiagramEditor();

  ///
  void Draw(const coreui::Diagram &diagram);

 private:
  ///
  std::unique_ptr<ne::EditorContext, void (*)(ne::EditorContext *)> context_;
};
}  // namespace esc::draw

#endif  // VH_DRAW_DIAGRAM_EDITOR_H_
