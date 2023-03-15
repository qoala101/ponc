#include "draw_node_popup.h"

#include <string>
#include <vector>

#include "imgui.h"
#include "imgui_node_editor.h"

namespace esc::draw {
namespace {
///
auto GetSelectedNodes() {
  auto selected_nodes = std::vector<ne::NodeId>{};
  selected_nodes.resize(ne::GetSelectedObjectCount());

  const auto num_selected_nodes = ne::GetSelectedNodes(
      selected_nodes.data(), static_cast<int>(selected_nodes.size()));
  selected_nodes.resize(num_selected_nodes);

  return selected_nodes;
}

///
auto GetTitle(const std::vector<ne::NodeId>& nodes) {
  if (nodes.empty()) {
    return std::string{};
  }

  const auto num_nodes = nodes.size();

  if (num_nodes == 1) {
    return "Node #" + std::to_string(nodes.front().Get());
  }

  return "Nodes (" + std::to_string(nodes.size()) + ")";
}
}  // namespace

///
void NodePopup::Draw(const Callbacks& callbacks) {
  const auto selected_nodes =
      IsOpened() ? GetSelectedNodes() : std::vector<ne::NodeId>{};
  const auto title = GetTitle(selected_nodes);
  const auto content_scope = DrawContentScope(title);

  if (!IsOpened()) {
    return;
  }

  if (ImGui::MenuItem("Delete")) {
    callbacks.delete_selected(selected_nodes);
  }

  if (ImGui::MenuItem("Delete With Links")) {
    callbacks.delete_with_links_selected(selected_nodes);
  }

  if (selected_nodes.size() > 1) {
    return;
  }

  if (ImGui::BeginMenu("Replace With")) {
    ImGui::EndMenu();
  }

  if (ImGui::BeginMenu("Fill With")) {
    ImGui::EndMenu();
  }
}
}  // namespace esc::draw