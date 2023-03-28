#include "draw_calculator_view.h"

#include <algorithm>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <set>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "core_diagram.h"
#include "core_i_family.h"
#include "core_i_node.h"
#include "core_link.h"
#include "core_project.h"
#include "coreui_diagram.h"
#include "coreui_flow_tree.h"
#include "coreui_i_family_traits.h"
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

void DrawFamily(const core::IFamily& family, flow::FamilyFlow& family_flow) {
  ImGui::TableNextRow();

  ImGui::TableNextColumn();
  ImGui::TextUnformatted(family.CreateUiTraits()->GetLabel().c_str());

  ImGui::TableNextColumn();
  ImGui::SetNextItemWidth(-std::numeric_limits<float>::min());
  ImGui::InputInt(IdLabel(family.GetId()).c_str(), &family_flow.cost);
}

void TraverseDepthFirst(
    const flow::TreeNodeEx& tree_node,
    const std::invocable<const flow::TreeNodeEx&> auto& visitor_before_children,
    const std::invocable<const flow::TreeNodeEx&> auto&
        visitor_after_children) {
  visitor_before_children(tree_node);

  for (const auto& child : tree_node.child_nodes) {
    TraverseDepthFirst(child.second, visitor_before_children,
                       visitor_after_children);
  }

  visitor_after_children(tree_node);
}

auto MakeDiagram(core::Project& project, const flow::Diagram& diagram) {
  auto new_diagram = core::Diagram{"new diag"};
  auto parent_stack =
      std::stack<std::pair<const flow::TreeNodeEx*, const core::INode*>>{};

  TraverseDepthFirst(
      diagram.output_tree,
      [&project, &new_diagram, &parent_stack](const auto& tree_node) {
        auto& id_generator = project.GetIdGenerator();

        const auto& family =
            core::Project::FindFamily(project, tree_node.family_id);
        const auto& node =
            new_diagram.EmplaceNode(family.CreateNode(id_generator));

        if (!parent_stack.empty()) {
          const auto& [parent_tree_node, parent_node] = parent_stack.top();

          for (const auto& child_node : parent_tree_node->child_nodes) {
            if (&child_node.second != &tree_node) {
              continue;
            }

            const auto parent_output_pins = parent_node->GetOutputPinIds();

            Expects(static_cast<int>(parent_output_pins.size()) >
                    child_node.first);
            const auto parent_output_pin_to_this =
                parent_output_pins[child_node.first];

            const auto input_pin = node.GetInputPinId();
            Expects(input_pin.has_value());

            const auto link =
                core::Link{.id = id_generator.Generate<ne::LinkId>(),
                           .start_pin_id = parent_output_pin_to_this,
                           .end_pin_id = *input_pin};

            new_diagram.EmplaceLink(link);

            break;
          }
        }

        parent_stack.emplace(&tree_node, &node);
      },
      [&parent_stack](const auto&) { parent_stack.pop(); });

  return new_diagram;
}
}  // namespace

auto CalculatorView::GetLabel() const -> std::string { return "Calculator"; }

void CalculatorView::Draw(core::Project& project,
                          const coreui::Diagram& diagram,
                          std::optional<const coreui::Node*> node,
                          const Callbacks& callbacks) {
  const auto content_scope = DrawContentScope();

  if (!IsOpened()) {
    return;
  }

  const auto has_node = node.has_value();
  auto calculate_pressed = false;

  {
    const auto disable_scope = DisableIf(!has_node);

    if (ImGui::Button("Calculate")) {
      calculate_pressed = true;
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

      for (const auto& family : project.GetFamilies()) {
        if (const auto family_is_default = family->GetType().has_value()) {
          continue;
        }

        const auto sample_node = family->CreateSampleNode();

        if (!sample_node->GetInputPinId().has_value() ||
            sample_node->GetOutputPinIds().empty()) {
          continue;
        }

        const auto output_pin_flows =
            sample_node->GetInitialFlow().output_pin_flows;

        auto output_pin_flows_vector = std::vector<float>{};
        std::transform(output_pin_flows.begin(), output_pin_flows.end(),
                       std::back_inserter(output_pin_flows_vector),
                       [](const auto& pair) { return pair.second; });

        const auto family_id = family->GetId().Get();
        auto& family_flow =
            family_flows_.contains(family_id)
                ? family_flows_.at(family_id)
                : family_flows_
                      .emplace(family_id,
                               flow::FamilyFlow{.output_pin_flows = std::move(
                                                    output_pin_flows_vector),
                                                .cost = 10})
                      .first->second;

        DrawFamily(*family, family_flow);
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

  if (calculate_pressed) {
    auto root = flow::TreeNodeEx{.family_id = (*node)->GetNode().GetFamilyId()};
    auto child_index = 0;

    for (const auto& child : (*node)->GetTreeNode().child_nodes) {
      root.child_nodes.emplace(
          child_index++,
          flow::TreeNodeEx{.family_id = child.node->GetNode().GetFamilyId()});
    }

    auto result =
        flow::Calculate({.input_tree = root, .family_flows = family_flows_});

    auto diagrams = std::vector<core::Diagram>{};

    std::transform(result.begin(), result.end(), std::back_inserter(diagrams),
                   [&project](const auto& diagram) {
                     return MakeDiagram(project, diagram);
                   });

    callbacks.calculated_diagrams(std::move(diagrams));
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