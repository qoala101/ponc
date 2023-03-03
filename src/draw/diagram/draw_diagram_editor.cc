#include "draw_diagram_editor.h"

#include <functional>

#include "coreui_diagram.h"
#include "cpp_scope.h"
#include "draw_handmade_link.h"
#include "draw_link.h"
#include "draw_link_creation.h"
#include "draw_main_window.h"
#include "draw_node.h"
#include "imgui_node_editor.h"

namespace esc::draw {
///
DiagramEditor::DiagramEditor()
    : context_{ne::CreateEditor(), &ne::DestroyEditor} {
  ne::SetCurrentEditor(context_.get());
}

///
void DiagramEditor::Draw(const coreui::Diagram &diagram) {
  ne::Begin("DiagramEditor");
  ne::End();
}
}  // namespace esc::draw