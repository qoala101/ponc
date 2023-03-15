#include "draw_diagram_editor.h"

#include <functional>
#include <memory>

#include "core_diagram.h"
#include "coreui_diagram.h"
#include "cpp_scope.h"
#include "draw_create_node_popup.h"
#include "draw_link.h"
#include "draw_linker.h"
#include "draw_node.h"
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
  item_deleter_.UnregisterDeletedItems(diagram.GetDiagram());

  for (auto &node : diagram.GetNodes()) {
    DrawNode(node);
  }

  for (const auto &link : diagram.GetLinks()) {
    DrawLink(link);
  }

  linker_.Draw(diagram.GetLinker(),
               {.new_node_requested_at =
                    [&create_node_popup = create_node_popup_](const auto &pos) {
                      create_node_popup.SetPos(pos);
                      create_node_popup.Open();
                    }});

  OpenPopupsIfRequested(diagram.GetDiagram());
  DrawPopups(diagram);

  item_deleter_.DeleteUnregisteredItems(diagram);
  ne::End();
}

///
void DiagramEditor::OpenPopupsIfRequested(const core::Diagram &diagram) {
  const auto popup_pos = ImGui::GetMousePos();

  ne::Suspend();
  const auto resume_scope = cpp::Scope{[]() { ne::Resume(); }};

  if (ne::ShowBackgroundContextMenu()) {
    create_node_popup_.SetPos(popup_pos);
    create_node_popup_.Open();
    return;
  }

  auto node_id = ne::NodeId{};
  auto requested_node_popup = false;

  if (ne::ShowNodeContextMenu(&node_id)) {
    requested_node_popup = true;
  } else {
    auto pin_id = ne::PinId{};

    if (ne::ShowPinContextMenu(&pin_id)) {
      node_id = core::Diagram::FindPinNode(diagram, pin_id).GetId();
      requested_node_popup = true;
    }
  }

  if (requested_node_popup) {
    if (!ne::IsNodeSelected(node_id)) {
      ne::SelectNode(node_id);
    }

    node_popup_.Open();
    return;
  }

  auto link_id = ne::LinkId{};

  if (ne::ShowLinkContextMenu(&link_id)) {
    if (!ne::IsLinkSelected(link_id)) {
      ne::SelectLink(link_id);
    }

    link_popup_.Open();
  }
}

///
void DiagramEditor::DrawPopups(coreui::Diagram &diagram) {
  DrawCreateNodePopup(diagram);

  node_popup_.Draw({.delete_selected =
                        [&diagram](const auto &node_ids) {
                          for (const auto node_id : node_ids) {
                            diagram.DeleteNode(node_id);
                          }
                        },
                    .delete_with_links_selected =
                        [&diagram](const auto &node_ids) {
                          for (const auto node_id : node_ids) {
                            diagram.DeleteNodeWithLinks(node_id);
                          }
                        }});

  link_popup_.Draw({.delete_selected = [&diagram](const auto &link_ids) {
    for (const auto link_id : link_ids) {
      diagram.DeleteLink(link_id);
    }
  }});
}

///
void DiagramEditor::DrawCreateNodePopup(coreui::Diagram &diagram) {
  if (!create_node_popup_.IsOpened()) {
    return;
  }

  auto &linker = diagram.GetLinker();
  const auto &family_groups = diagram.GetFamilyGroups();

  if (!linker.IsCreatingNode()) {
    create_node_popup_.Draw(family_groups,
                            {.node_created = [&diagram](auto node) {
                              diagram.AddNode(std::move(node));
                            }});
    return;
  }

  create_node_popup_.Draw(
      family_groups,
      {.is_family_enabled =
           [&linker](const auto &family) {
             return linker.CanConnectToNode(*family.CreateNode());
           },
       .closed = [&linker]() { linker.DiscardNewNode(); },
       .node_created =
           [&diagram, &linker](auto node) {
             linker.AcceptNewNode(*node);
             diagram.AddNode(std::move(node));
           }});
}
}  // namespace esc::draw