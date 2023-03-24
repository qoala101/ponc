#include "draw_calculator_view.h"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <set>
#include <string>
#include <unordered_map>
#include <unordered_set>

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
void DrawMinInput(const coreui::TreeNode& tree_node) {
  // static float f{};
  // ImGui::InputFloat("", &f, 0, 0, "%.3f");
}

void DrawMaxInput(const coreui::TreeNode& tree_node) {
  // static float f{};
  // ImGui::InputFloat("", &f, 0, 0, "%.3f");
}

void DrawFamily(const coreui::Family& family, flow::FamilyFlow& family_flow) {
  ImGui::TableNextRow();

  ImGui::TableNextColumn();
  ImGui::TextUnformatted(family.GetLabel().c_str());

  const auto family_id = family.GetFamily().GetId();

  ImGui::TableNextColumn();
  ImGui::InputInt((IdLabel(family_id) + "Count").c_str(), &family_flow.count);

  ImGui::TableNextColumn();
  ImGui::InputFloat((IdLabel(family_id) + "Cost").c_str(), &family_flow.cost, 0,
                    0, "%.3f");
}
}  // namespace

auto CalculatorView::GetLabel() const -> std::string { return "Calculator"; }

void CalculatorView::Draw(const coreui::Diagram& diagram,
                          std::optional<const coreui::Node*> node) {
  const auto content_scope = DrawContentScope();

  if (!IsOpened()) {
    calculator_input_.reset();
    return;
  }

  if (!node.has_value()) {
    ImGui::TextUnformatted("Select single Node.");
    ImGui::Separator();

    calculator_input_.reset();
    return;
  }

  const auto node_id = (*node)->GetNode().GetId();

  if (calculator_input_.has_value() &&
      calculator_input_->tree_node.node_id != node_id) {
    calculator_input_.reset();
  }

  if (!calculator_input_.has_value()) {
    const auto flow_tree = flow::BuildFlowTree(diagram.GetDiagram());
    // const auto node_flows = flow::CalculateNodeFlows(
    //     flow_tree, [&diagram = diagram.GetDiagram()](const auto node_id) {
    //       return core::Diagram::FindNode(diagram, node_id).GetInitialFlow();
    //     });

    calculator_input_ = flow::CalculatorInput{
        .tree_node = flow::FindTreeNode(flow_tree, node_id)};
  }

  ImGui::TextUnformatted((*node)->GetData().label.c_str());

  // NOLINTBEGIN(*-signed-bitwise)
  const auto table_flags =
      ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable |
      ImGuiTableFlags_Hideable | ImGuiTableFlags_ContextMenuInBody |
      ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersOuter;
  // NOLINTEND(*-signed-bitwise)

  if (ImGui::CollapsingHeader("Required Inputs")) {
    if (ImGui::BeginTable("Required Inputs", 3, table_flags)) {
      ImGui::TableSetupColumn("Node");
      ImGui::TableSetupColumn("Min");
      ImGui::TableSetupColumn("Max");
      ImGui::TableHeadersRow();

      DrawTreeNode((*node)->GetTreeNode(), true, false,
                   {&DrawMinInput, &DrawMaxInput});

      ImGui::EndTable();
    }
  }

  if (ImGui::CollapsingHeader("Available Nodes")) {
    if (ImGui::BeginTable("Available Nodes", 3, table_flags)) {
      ImGui::TableSetupColumn("Node Type");
      ImGui::TableSetupColumn("Count");
      ImGui::TableSetupColumn("Cost");
      ImGui::TableHeadersRow();

      auto& available_node_flows = calculator_input_->available_node_flows;

      for (const auto& family_group : diagram.GetFamilyGroups()) {
        for (const auto& family : family_group.families) {
          const auto& core_family = family.GetFamily();

          if (family.GetFamily().GetType().has_value()) {
            continue;
          }

          const auto sample_node = family.GetFamily().CreateSampleNode();

          if (!sample_node->GetInputPinId().has_value() ||
              sample_node->GetOutputPinIds().empty()) {
            continue;
          }

          auto* family_flow = static_cast<flow::FamilyFlow*>(nullptr);
          const auto family_id = core_family.GetId().Get();

          if (available_node_flows.contains(family_id)) {
            family_flow = &available_node_flows.at(family_id);
          } else {
            std::cout << "emplca\n";
            family_flow =
                &calculator_input_->available_node_flows
                     .emplace(core_family.GetId(),
                              flow::FamilyFlow{
                                  .node_flow = sample_node->GetInitialFlow(),
                                  .count = 1000,
                                  .cost = 100})
                     .first->second;
          }

          DrawFamily(family, *family_flow);
        }
      }

      ImGui::EndTable();
    }
  }

  if (ImGui::CollapsingHeader("Controls")) {
    if (ImGui::Button("Calculate")) {
    }
  }
}
}  // namespace vh::ponc::draw