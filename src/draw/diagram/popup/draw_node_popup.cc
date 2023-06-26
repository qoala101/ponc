/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_node_popup.h"

#include <imgui.h>
#include <imgui_node_editor.h>

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "core_diagram.h"
#include "core_i_node.h"
#include "coreui_i_node_traits.h"
#include "coreui_native_facade.h"

namespace vh::ponc::draw {
namespace {
///
auto GetSizeText(int size) { return "s (" + std::to_string(size) + ")"; }

///
auto GetSizeText(const std::vector<ne::NodeId>& nodes) {
  if (nodes.empty()) {
    return std::string{};
  }

  if (nodes.size() == 1) {
    return " #" + std::to_string(nodes.front().Get());
  }

  return GetSizeText(static_cast<int>(nodes.size()));
}

///
auto GetTitle(const std::vector<ne::NodeId>& nodes,
              const std::vector<ne::NodeId>& areas) {
  if (nodes.empty()) {
    if (areas.empty()) {
      return std::string{};
    }

    return "Area" + GetSizeText(areas);
  }

  if (!areas.empty() || (nodes.size() > 1)) {
    return "Node" + GetSizeText(static_cast<int>(nodes.size()));
  }

  return "Node" + GetSizeText(nodes);
}

///
void DrawNodeActions(core::INode& node) {
  const auto node_traits = node.CreateUiTraits();
  const auto node_action_names = node_traits->GetActionNames();

  if (node_action_names.empty()) {
    return;
  }

  ImGui::Separator();

  for (const auto& action_name : node_action_names) {
    if (ImGui::MenuItem(action_name.c_str())) {
      node_traits->ExecuteAction(action_name);
    }
  }
}
}  // namespace

///
void NodePopup::Draw(coreui::Diagram& diagram) {
  const auto [selected_nodes, selected_areas] =
      IsOpened()
          ? coreui::NativeFacade::GetSelectedNodesAndAreas()
          : std::pair<std::vector<ne::NodeId>, std::vector<ne::NodeId>>{};
  const auto title = GetTitle(selected_nodes, selected_areas);
  const auto content_scope = DrawContentScope(title);

  if (!IsOpened()) {
    return;
  }

  if (selected_nodes.empty()) {
    area_popup_.Draw(selected_areas, diagram);
    return;
  }

  auto delete_selected = false;

  if (ImGui::MenuItem("Delete")) {
    delete_selected = true;

    for (const auto node_id : selected_nodes) {
      diagram.DeleteNode(node_id);
    }
  }

  if (ImGui::MenuItem("Delete With Links")) {
    delete_selected = true;

    for (const auto node_id : selected_nodes) {
      diagram.DeleteNodeWithLinks(node_id);
    }
  }

  if (delete_selected) {
    for (const auto node_id : selected_areas) {
      diagram.DeleteArea(node_id);
    }
  }

  ImGui::Separator();

  if (ImGui::MenuItem("Tree Select")) {
    diagram.TreeSelect(selected_nodes);
  }

  if (ImGui::MenuItem("Tree Arrange")) {
    diagram.TreeArrange(selected_nodes);
  }

  if ((selected_nodes.size() > 1) || !selected_areas.empty()) {
    return;
  }

  auto& node =
      core::Diagram::FindNode(diagram.GetDiagram(), selected_nodes.front());

  DrawNodeActions(node);
}
}  // namespace vh::ponc::draw