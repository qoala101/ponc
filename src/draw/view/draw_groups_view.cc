#include "draw_groups_view.h"

#include <unordered_set>

#include "core_group.h"
#include "core_i_node.h"
#include "coreui_i_node_drawer.h"
#include "cpp_scope.h"
#include "imgui.h"
#include "imgui_node_editor.h"

namespace esc::draw {
namespace {
void DisplayNode(State& state, core::INode& node) {
  ImGui::TableNextRow();
  ImGui::TableNextColumn();

  const auto drawer = node.CreateDrawer(state);

  const auto node_id = node.GetId();
  const auto draw_flags = ImGuiTreeNodeFlags_Leaf;

  const auto node_is_open = ImGui::TreeNodeEx(
      std::string{"##" + std::to_string(static_cast<uintptr_t>(node_id.Get()))}
          .c_str(),
      draw_flags);

  ImGui::SameLine();

  const auto selectedNodes = state.diagram_.GetSelectedNodeIds();
  auto isSelected = std::find(selectedNodes.begin(), selectedNodes.end(),
                              node_id) != selectedNodes.end();
  if (ImGui::Selectable(
          std::string{drawer->GetLabel() + "##" +
                      std::to_string(static_cast<uintptr_t>(node_id.Get()))}
              .c_str(),
          &isSelected)) {
    auto& io = ImGui::GetIO();

    if (io.KeyCtrl) {
      if (isSelected)
        ne::SelectNode(node_id, true);
      else
        ne::DeselectNode(node_id);
    } else
      ne::SelectNode(node_id, false);

    ne::NavigateToSelection();
  }

  if (node_is_open) {
    ImGui::TreePop();
  }
}

void DisplayGroup(State& state, core::Group& group) {
  ImGui::TableNextRow();
  ImGui::TableNextColumn();

  const auto nodes = group.GetNodes();
  const auto has_children = !nodes.empty();
  const auto draw_flags =
      has_children ? (ImGuiTreeNodeFlags_DefaultOpen) : ImGuiTreeNodeFlags_Leaf;

  const auto node_is_open =
      ImGui::TreeNodeEx(std::string{"##" + group.name_}.c_str(), draw_flags);

  ImGui::SameLine();

  const auto group_node_ids = [&nodes]() {
    auto group_node_ids = std::unordered_set<uintptr_t>{};

    for (const auto& node : nodes) {
      group_node_ids.emplace(node->GetId().Get());
    }

    return group_node_ids;
  }();

  const auto selectedNodes = state.diagram_.GetSelectedNodeIds();
  const auto selected_node_ids = [&selectedNodes]() {
    auto selected_node_ids = std::unordered_set<uintptr_t>{};

    for (const auto& node : selectedNodes) {
      selected_node_ids.emplace(node.Get());
    }

    return selected_node_ids;
  }();

  auto isSelected = group_node_ids == selected_node_ids;

  if (ImGui::Selectable(group.name_.c_str(), &isSelected)) {
    auto& io = ImGui::GetIO();

    if (io.KeyCtrl) {
      if (isSelected) {
        for (const auto& node : nodes) {
          ne::SelectNode(node->GetId(), true);
        }
      } else {
        for (const auto& node : nodes) {
          ne::DeselectNode(node->GetId());
        }
      }
    } else {
      ne::ClearSelection();

      for (const auto& node : nodes) {
        ne::SelectNode(node->GetId(), true);
      }
    }

    ne::NavigateToSelection();
  }

  if (node_is_open) {
    if (has_children) {
      for (auto& node : nodes) {
        DisplayNode(state, *node);
      }
    }

    ImGui::TreePop();
  }
}
}  // namespace

void DrawGroupsView(State& state) {
  if (!state.DRAW_.groups_view_visible) {
    return;
  }

  {
    const auto window_scope = cpp::Scope{[]() { ImGui::End(); }};

    if (ImGui::Begin("Groups", &state.DRAW_.groups_view_visible)) {
      const auto table_flags =
          ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable |
          ImGuiTableFlags_Hideable | ImGuiTableFlags_ContextMenuInBody |
          ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersOuter |
          ImGuiTableFlags_ScrollY;

      if (ImGui::BeginTable("Groups", 1, table_flags)) {
        const auto table_scope = cpp::Scope{[]() { ImGui::EndTable(); }};

        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        auto& groups = state.diagram_.GetGroups();

        for (auto& group : groups) {
          DisplayGroup(state, group);
        }
      }
    }
  }
}
}  // namespace esc::draw