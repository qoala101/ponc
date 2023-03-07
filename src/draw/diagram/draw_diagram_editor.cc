#include "draw_diagram_editor.h"

#include <functional>
#include <memory>

#include "coreui_diagram.h"
#include "draw_link.h"
#include "draw_link_creation.h"
#include "draw_nodes.h"
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

  link_creation_.Draw(diagram.GetLinkCreation(), diagram.GetFamilyGroups(),
                      {.get_pin_tip_pos = [&nodes = nodes_](auto pin_id) {
                        return nodes.GetDrawnPinTipPos(pin_id);
                      }});

  nodes_.Draw(diagram.GetNodes());

  for (const auto &link : diagram.GetLinks()) {
    DrawLink(link);
  }

  DrawPopups(diagram);

  ne::End();
}

///
void DiagramEditor::DrawPopups(coreui::Diagram &diagram) {
  const auto popup_pos = ImGui::GetMousePos();

  ne::Suspend();

  if (ne::ShowBackgroundContextMenu()) {
    create_node_popup_.Open();
  }

  ne::Resume();

  create_node_popup_.Draw(diagram.GetFamilyGroups(),
                          {.node_created = [&diagram, &popup_pos](auto node) {
                            node->SetPos(popup_pos);
                            diagram.EmplaceNode(std::move(node));
                          }});
}
}  // namespace esc::draw