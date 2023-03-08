#include "draw_diagram_editor.h"

#include <functional>
#include <memory>

#include "coreui_diagram.h"
#include "cpp_scope.h"
#include "draw_create_node_popup.h"
#include "draw_link.h"
#include "draw_linking.h"
#include "draw_nodes.h"
#include "imgui_node_editor.h"

namespace esc::draw {
namespace {
///
auto GetConnectableFamilyGroups(
    const std::vector<coreui::FamilyGroup> &family_groups,
    const coreui::Linking &linking) {
  auto connectable_groups = std::vector<coreui::FamilyGroup>{};
  auto id_generator = core::IdGenerator{};

  for (const auto &family_group : family_groups) {
    for (const auto &family : family_group.families) {
      const auto fake_node = family.GetFamily().CreateNode(id_generator);

      if (!linking.CanConnectToNode(*fake_node)) {
        continue;
      }

      const auto connectable_group =
          std::find_if(connectable_groups.begin(), connectable_groups.end(),
                       [&family_group](const auto &group) {
                         return group.label == family_group.label;
                       });

      if (connectable_group == connectable_groups.end()) {
        connectable_groups.emplace_back(coreui::FamilyGroup{
            .label = family_group.label, .families = std::vector{family}});
        continue;
      }

      connectable_group->families.emplace_back(family);
    }
  }

  return connectable_groups;
}

///
void DeleteItemsIfRequested(coreui::Diagram &diagram) {
  if (ne::BeginDelete()) {
    auto link_id = ne::LinkId{};

    while (ne::QueryDeletedLink(&link_id)) {
      if (ne::AcceptDeletedItem()) {
        diagram.DeleteLink(link_id);
      }
    }

    auto node_id = ne::NodeId{};

    while (ne::QueryDeletedNode(&node_id)) {
      if (ne::AcceptDeletedItem()) {
        diagram.DeleteNode(node_id);
      }
    }
  }

  ne::EndDelete();
}
}  // namespace

///
void DiagramEditor::Draw(coreui::Diagram &diagram) {
  ne::Begin("DiagramEditor");

  linking_.Draw(
      diagram.GetLinking(),
      {.get_pin_tip_pos =
           [&nodes = nodes_](auto pin_id) {
             return nodes.GetDrawnPinTipPos(pin_id);
           },
       .new_node_requested_at =
           [&create_node_popup = create_node_popup_](const auto &pos) {
             create_node_popup.SetPos(pos);
             create_node_popup.Open();
           }});

  nodes_.Draw(diagram.GetNodes());

  for (const auto &link : diagram.GetLinks()) {
    DrawLink(link);
  }

  DeleteItemsIfRequested(diagram);
  OpenPopupsIfRequested();
  DrawPopups(diagram);

  ne::End();
}

///
void DiagramEditor::OpenPopupsIfRequested() {
  const auto popup_pos = ImGui::GetMousePos();

  ne::Suspend();
  const auto resume_scope = cpp::Scope{[]() { ne::Resume(); }};

  if (ne::ShowBackgroundContextMenu()) {
    create_node_popup_.SetPos(popup_pos);
    create_node_popup_.Open();
    return;
  }

  auto node_id = ne::NodeId{};

  if (ne::ShowNodeContextMenu(&node_id)) {
    node_popup_.SetNodeId(node_id);
    node_popup_.Open();
    return;
  }

  auto link_id = ne::LinkId{};

  if (ne::ShowLinkContextMenu(&link_id)) {
    link_popup_.SetLinkId(link_id);
    link_popup_.Open();
  }
}

///
void DiagramEditor::DrawPopups(coreui::Diagram &diagram) {
  DrawCreateNodePopup(diagram);

  node_popup_.Draw({.node_deleted = [&diagram](auto node_id) {
    diagram.DeleteNode(node_id);
  }});

  link_popup_.Draw({.link_deleted = [&diagram](auto link_id) {
    diagram.DeleteLink(link_id);
  }});
}

///
void DiagramEditor::DrawCreateNodePopup(coreui::Diagram &diagram) {
  if (!create_node_popup_.IsOpened()) {
    return;
  }

  auto &linking = diagram.GetLinking();
  const auto &family_groups = diagram.GetFamilyGroups();

  if (linking.IsCreatingNode()) {
    create_node_popup_.Draw(
        GetConnectableFamilyGroups(family_groups, linking),
        {.closed = [&linking]() { linking.DiscardNewNode(); },
         .node_created =
             [&diagram, &linking](auto node) {
               linking.AcceptNewNode(*node);
               diagram.AddNode(std::move(node));
             }});
    return;
  }

  create_node_popup_.Draw(family_groups,
                          {.node_created = [&diagram](auto node) {
                            diagram.AddNode(std::move(node));
                          }});
}
}  // namespace esc::draw