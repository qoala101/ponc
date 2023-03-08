#include "draw_diagram_editor.h"

#include <functional>
#include <memory>

#include "coreui_diagram.h"
#include "draw_create_node_popup.h"
#include "draw_link.h"
#include "draw_link_creation.h"
#include "draw_nodes.h"
#include "imgui_node_editor.h"

namespace esc::draw {
namespace {
///
auto GetConnectableFamilyGroups(
    const std::vector<coreui::FamilyGroup> &family_groups,
    const coreui::LinkCreation &link_creation) {
  auto connectable_groups = std::vector<coreui::FamilyGroup>{};
  auto id_generator = core::IdGenerator{};

  for (const auto &family_group : family_groups) {
    for (const auto &family : family_group.families) {
      const auto fake_node = family.GetFamily().CreateNode(id_generator);

      if (!link_creation.CanConnectToNode(*fake_node)) {
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
}  // namespace

///
DiagramEditor::DiagramEditor()
    : context_{ne::CreateEditor(), &ne::DestroyEditor} {
  ne::SetCurrentEditor(context_.get());
}

///
void DiagramEditor::Draw(coreui::Diagram &diagram) {
  ne::Begin("DiagramEditor");

  link_creation_.Draw(
      diagram.GetLinkCreation(),
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

  DrawPopups(diagram);

  ne::End();
}

///
void DiagramEditor::DrawPopups(coreui::Diagram &diagram) {
  const auto popup_pos = ImGui::GetMousePos();

  ne::Suspend();

  if (ne::ShowBackgroundContextMenu()) {
    create_node_popup_.SetPos(popup_pos);
    create_node_popup_.Open();
  }

  ne::Resume();

  DrawCreateNodePopup(diagram);
}

///
void DiagramEditor::DrawCreateNodePopup(coreui::Diagram &diagram) {
  if (!create_node_popup_.IsOpened()) {
    return;
  }

  auto &link_creation = diagram.GetLinkCreation();
  const auto &family_groups = diagram.GetFamilyGroups();

  if (link_creation.IsCreatingNode()) {
    create_node_popup_.Draw(
        GetConnectableFamilyGroups(family_groups, link_creation),
        {.closed = [&link_creation]() { link_creation.DiscardNewNode(); },
         .node_created =
             [&diagram, &link_creation](auto node) {
               link_creation.AcceptNewNode(*node);
               diagram.EmplaceNode(std::move(node));
             }});
    return;
  }

  create_node_popup_.Draw(family_groups,
                          {.node_created = [&diagram](auto node) {
                            diagram.EmplaceNode(std::move(node));
                          }});
}
}  // namespace esc::draw