#include "draw_families_view.h"

#include <unordered_set>

#include "core_i_family.h"
#include "cpp_scope.h"
#include "draw_i_family_drawer.h"
#include "draw_i_node_drawer.h"
#include "esc_state.h"
#include "imgui.h"

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

  const auto selectedNodes = state.app_.GetDiagram().GetSelectedNodeIds();
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

  ImGui::TableNextColumn();
  const auto flow = state.flow_calculator_.GetCalculatedFlow(node);

  if (flow.input_pin_flow.has_value()) {
    ImGui::TextColored(state.GetColorForFlowValue(flow.input_pin_flow->second),
                       "%.3f", flow.input_pin_flow->second);
  }

  if (node_is_open) {
    ImGui::TreePop();
  }
}

void DisplayFamily(State& state, core::IFamily& family) {
  ImGui::TableNextRow();
  ImGui::TableNextColumn();

  const auto nodes = family.GetNodes();
  const auto has_children = !nodes.empty();
  const auto draw_flags =
      has_children ? (ImGuiTreeNodeFlags_DefaultOpen) : ImGuiTreeNodeFlags_Leaf;

  const auto node_is_open = ImGui::TreeNodeEx(
      std::string{"##" + family.CreateDrawer()->GetLabel()}.c_str(),
      draw_flags);

  ImGui::SameLine();

  const auto group_node_ids = [&nodes]() {
    auto group_node_ids = std::unordered_set<uintptr_t>{};

    for (const auto& node : nodes) {
      group_node_ids.emplace(node->GetId().Get());
    }

    return group_node_ids;
  }();

  const auto selectedNodes = state.app_.GetDiagram().GetSelectedNodeIds();
  const auto selected_node_ids = [&selectedNodes]() {
    auto selected_node_ids = std::unordered_set<uintptr_t>{};

    for (const auto& node : selectedNodes) {
      selected_node_ids.emplace(node.Get());
    }

    return selected_node_ids;
  }();

  auto isSelected = group_node_ids == selected_node_ids;

  if (ImGui::Selectable(family.CreateDrawer()->GetLabel().c_str(),
                        &isSelected)) {
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

void DrawFamiliesView(State& state) {
  if (!state.DRAW_.families_view_visible) {
    return;
  }

  {
    const auto window_scope = cpp::Scope{[]() { ImGui::End(); }};

    if (ImGui::Begin("Families", &state.DRAW_.families_view_visible)) {
      const auto& families = state.app_.GetDiagram().GetFamilies();
      const auto table_flags =
          ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable |
          ImGuiTableFlags_Hideable | ImGuiTableFlags_ContextMenuInBody |
          ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersOuter |
          ImGuiTableFlags_ScrollY;

      if (ImGui::BeginTable("Families", 2, table_flags)) {
        const auto table_scope = cpp::Scope{[]() { ImGui::EndTable(); }};

        ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 300);
        ImGui::TableSetupColumn("Input");
        ImGui::TableHeadersRow();

        for (const auto& family : state.app_.GetDiagram().GetFamilies()) {
          if (!family->GetNodes().empty()) {
            DisplayFamily(state, *family);
          }
        }
      }
    }
  }
}
}  // namespace esc::draw