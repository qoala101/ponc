#include "draw_diagram_editor.h"

#include <functional>
#include <memory>

#include "coreui_diagram.h"
#include "cpp_scope.h"
#include "imgui_node_editor.h"

namespace esc::draw {
///
DiagramEditor::DiagramEditor()
    : context_{ne::CreateEditor(), &ne::DestroyEditor} {
  ne::SetCurrentEditor(context_.get());
}

///
void DiagramEditor::Draw(coreui::Diagram &diagram) {
  ne::Begin("DiagramEditor");

  ShowPopupsIfRequested();
  DrawPopups(diagram);

  ne::End();
}

void DiagramEditor::ShowPopupsIfRequested() {
  const auto popup_pos = ImGui::GetMousePos();

  ne::Suspend();
  const auto resume_scope = cpp::Scope{[]() { ne::Resume(); }};

  if (ne::ShowBackgroundContextMenu()) {
    background_popup_.SetPos(popup_pos);
    background_popup_.Show();
    return;
  }
}

void DiagramEditor::DrawPopups(coreui::Diagram &diagram) {
  background_popup_.Draw(diagram.GetFamilyGroups());
}
}  // namespace esc::draw