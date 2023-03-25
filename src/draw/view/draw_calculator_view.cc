#include "draw_calculator_view.h"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "core_diagram.h"
#include "core_i_family.h"
#include "coreui_diagram.h"
#include "coreui_family.h"
#include "coreui_flow_tree.h"
#include "cpp_assert.h"
#include "draw_id_label.h"
#include "draw_tree_node.h"
#include "flow_algorithms.h"
#include "flow_calculator.h"
#include "flow_tree.h"
#include "flow_tree_traversal.h"
#include "imgui.h"
#include "imgui_node_editor.h"

namespace vh::ponc::draw {
namespace {
auto DisableIf(bool condition) {
  if (!condition) {
    return cpp::ScopeFunction{[]() {}};
  }

  ImGui::PushDisabled();
  return cpp::ScopeFunction{[]() { ImGui::PopDisabled(); }};
}

void DrawFamily(const coreui::Family& family, int& cost) {
  ImGui::TableNextRow();

  ImGui::TableNextColumn();
  ImGui::TextUnformatted(family.GetLabel().c_str());

  const auto family_id = family.GetFamily().GetId();

  ImGui::TableNextColumn();
  ImGui::SetNextItemWidth(-std::numeric_limits<float>::min());
  ImGui::InputInt((IdLabel(family_id) + "Count").c_str(), &cost);
}
}  // namespace

auto CalculatorView::GetLabel() const -> std::string { return "Calculator"; }

void CalculatorView::Draw(const coreui::Diagram& diagram,
                          std::optional<const coreui::Node*> node,
                          const Callbacks& callbacks) {
  const auto content_scope = DrawContentScope();

  if (!IsOpened()) {
    return;
  }

  const auto has_node = node.has_value();

  {
    const auto disable_scope = DisableIf(!has_node);

    if (ImGui::Button("Calculate")) {
      auto diagrams = std::vector<core::Diagram>{};
      diagrams.emplace_back("vova");
      diagrams.emplace_back("vova2");
      diagrams.emplace_back("vova3");

      callbacks.calculated_diagrams(std::move(diagrams));
    }
  }

  if (!has_node) {
    ImGui::SameLine();
    ImGui::TextUnformatted("Select root Node.");
  }

  // NOLINTBEGIN(*-signed-bitwise)
  const auto table_flags =
      ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable |
      ImGuiTableFlags_Hideable | ImGuiTableFlags_ContextMenuInBody |
      ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersOuter;
  // NOLINTEND(*-signed-bitwise)

  if (ImGui::CollapsingHeader("Node Cost", ImGuiTreeNodeFlags_DefaultOpen)) {
    if (ImGui::BeginTable("Node Cost", 2, table_flags)) {
      ImGui::TableSetupColumn("Node Type");
      ImGui::TableSetupColumn("Cost, $");
      ImGui::TableHeadersRow();

      for (const auto& family_group : diagram.GetFamilyGroups()) {
        for (const auto& family : family_group.families) {
          const auto& core_family = family.GetFamily();

          if (const auto family_is_default =
                  family.GetFamily().GetType().has_value()) {
            continue;
          }

          const auto sample_node = family.GetFamily().CreateSampleNode();

          if (!sample_node->GetInputPinId().has_value() ||
              sample_node->GetOutputPinIds().empty()) {
            continue;
          }

          const auto family_id = core_family.GetId().Get();
          auto& family_cost =
              family_costs_.contains(family_id)
                  ? family_costs_.at(family_id)
                  : family_costs_.emplace(family_id, 10).first->second;

          DrawFamily(family, family_cost);
        }
      }

      ImGui::EndTable();
    }
  }

  if (!has_node) {
    return;
  }

  const auto flow_tree = flow::BuildFlowTree(diagram.GetDiagram());
  auto tree_node = flow::FindTreeNode(flow_tree, (*node)->GetNode().GetId());

  if (tree_node != tree_node_) {
    tree_node_ = std::move(tree_node);
    required_inputs_.clear();
  }

  if (ImGui::CollapsingHeader("Required Inputs",
                              ImGuiTreeNodeFlags_DefaultOpen)) {
    if (ImGui::BeginTable("Required Inputs", 3, table_flags)) {
      ImGui::TableSetupColumn("Node");
      ImGui::TableSetupColumn("Min");
      ImGui::TableSetupColumn("Max");
      ImGui::TableHeadersRow();

      DrawTreeNode((*node)->GetTreeNode(), true, false,
                   {std::bind_front(&CalculatorView::DrawMinNodeInput, this),
                    std::bind_front(&CalculatorView::DrawMaxNodeInput, this)});

      ImGui::EndTable();
    }
  }
}

auto CalculatorView::GetNodeInputRange(ne::NodeId node_id) -> auto& {
  const auto node_id_value = node_id.Get();

  return required_inputs_.contains(node_id_value)
             ? required_inputs_.at(node_id_value)
             : required_inputs_.emplace(node_id, flow::InputRange{-22, -18})
                   .first->second;
}

void CalculatorView::DrawMinNodeInput(const coreui::TreeNode& tree_node) {
  if (!tree_node.child_nodes.empty()) {
    return;
  }

  ImGui::SetNextItemWidth(-std::numeric_limits<float>::min());

  const auto node_id = tree_node.node->GetNode().GetId();

  ImGui::InputFloat((IdLabel(node_id) + "Min").c_str(),
                    &GetNodeInputRange(node_id).min, 0, 0, "%.3f");
}

void CalculatorView::DrawMaxNodeInput(const coreui::TreeNode& tree_node) {
  if (!tree_node.child_nodes.empty()) {
    return;
  }

  ImGui::SetNextItemWidth(-std::numeric_limits<float>::min());

  const auto node_id = tree_node.node->GetNode().GetId();

  ImGui::InputFloat((IdLabel(node_id) + "Max").c_str(),
                    &GetNodeInputRange(node_id).max, 0, 0, "%.3f");
}
}  // namespace vh::ponc::draw