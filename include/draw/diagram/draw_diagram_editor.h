#ifndef VH_DRAW_DIAGRAM_EDITOR_H_
#define VH_DRAW_DIAGRAM_EDITOR_H_

#include <imgui_node_editor.h>

#include <memory>

#include "coreui_diagram.h"
#include "draw_background_popup.h"
#include "draw_link_creation.h"

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
  void ShowPopupsIfRequested();
  ///
  void DrawPopups(coreui::Diagram &diagram);

  ///
  std::unique_ptr<ne::EditorContext, void (*)(ne::EditorContext *)> context_;
  ///
  LinkCreation link_creation_{};
  ///
  BackgroundPopup background_popup_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_DIAGRAM_EDITOR_H_
