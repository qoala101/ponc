#include "draw_calculator_view.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <future>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <mutex>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "calc_calculator.h"
#include "calc_settings.h"
#include "calc_tree_node.h"
#include "core_diagram.h"
#include "core_i_family.h"
#include "core_i_node.h"
#include "core_link.h"
#include "core_project.h"
#include "coreui_i_family_traits.h"
#include "cpp_assert.h"
#include "draw_disable_if.h"
#include "draw_id_label.h"
#include "draw_settings_table_row.h"
#include "draw_table_flags.h"
#include "flow_algorithms.h"
#include "flow_tree_node.h"
#include "flow_tree_traversal.h"
#include "imgui.h"
#include "imgui_node_editor.h"

namespace vh::ponc::draw {
namespace {
///
auto GetClientFamilyId(core::Project& project) {
  const auto& families = project.GetFamilies();
  const auto client_family =
      std::find_if(families.cbegin(), families.cend(), [](const auto& family) {
        const auto& type = family->GetType();
        return type.has_value() && (*type == core::FamilyType::kClient);
      });

  Expects(client_family != families.end());
  return (*client_family)->GetId();
}

///
auto GetNodeOutputs(const core::INode& node) -> std::vector<float> {
  const auto& output_pins = node.GetOutputPinIds();

  if (output_pins.empty()) {
    return {};
  }

  const auto output_pin_flows = node.GetInitialFlow().output_pin_flows;

  auto outputs = std::vector<float>{};
  outputs.reserve(output_pins.size());

  std::transform(output_pins.begin(), output_pins.end(),
                 std::back_inserter(outputs),
                 [&output_pin_flows](const auto pin_id) {
                   const auto pin_id_value = pin_id.Get();
                   Expects(output_pin_flows.contains(pin_id_value));
                   return output_pin_flows.at(pin_id_value);
                 });

  return outputs;
}

///
auto AsFamilyNodes(const std::vector<std::unique_ptr<core::IFamily>>& families,
                   const std::vector<calc::FamilySettings>& family_settings) {
  auto family_nodes = std::vector<calc::TreeNode>{};

  for (const auto& settings : family_settings) {
    if (!settings.enabled) {
      continue;
    }

    const auto family = std::find_if(
        families.cbegin(), families.cend(), [&settings](const auto& family) {
          return family->GetId() == settings.family_id;
        });
    Expects(family != families.cend());

    const auto sample_node = (*family)->CreateSampleNode();
    auto outputs = GetNodeOutputs(*sample_node);

    family_nodes.emplace_back(settings.family_id, std::move(outputs),
                              settings.cost);
  }

  return family_nodes;
}

///
auto GetChildIndex(const flow::TreeNode& flow_tree, ne::NodeId child_id) {
  const auto child =
      std::find_if(flow_tree.child_nodes.cbegin(), flow_tree.child_nodes.cend(),
                   [child_id](const auto& child_node) {
                     return child_node.second.node_id == child_id;
                   });

  Expects(child != flow_tree.child_nodes.cend());
  return std::distance(flow_tree.child_nodes.begin(), child);
}

///
auto ToCalcTree(const flow::TreeNode& flow_tree, const core::Diagram& diagram) {
  auto calc_root = calc::TreeNode{};
  auto parent_stack =
      std::stack<std::pair<calc::TreeNode*, const flow::TreeNode*>>{};

  flow::TraverseDepthFirst(
      flow_tree,
      [&diagram, &calc_root, &parent_stack](const auto& flow_node) {
        const auto& node = core::Diagram::FindNode(diagram, flow_node.node_id);
        auto calc_node =
            calc::TreeNode{node.GetFamilyId(), GetNodeOutputs(node)};

        if (parent_stack.empty()) {
          calc_root = std::move(calc_node);
          parent_stack.emplace(&calc_root, &flow_node);
          return;
        }

        auto& [parent_calc_node, parent_flow_node] = parent_stack.top();

        const auto child_index =
            GetChildIndex(*parent_flow_node, flow_node.node_id);

        auto& child_node =
            parent_calc_node->EmplaceChild(child_index, std::move(calc_node));

        parent_stack.emplace(&child_node, &flow_node);
      },
      [&parent_stack](const auto&) { parent_stack.pop(); });

  return calc_root;
}

///
auto ToCalcTrees(const core::Diagram& diagram) -> std::vector<calc::TreeNode> {
  const auto flow_trees = flow::BuildFlowTrees(diagram);

  if (flow_trees.empty()) {
    return {};
  }

  auto calc_trees = std::vector<calc::TreeNode>{};
  calc_trees.reserve(flow_trees.size());

  std::transform(flow_trees.begin(), flow_trees.end(),
                 std::back_inserter(calc_trees),
                 [&diagram](const auto& flow_tree) {
                   return ToCalcTree(flow_tree, diagram);
                 });

  return calc_trees;
}

///
void DrawRequirements(calc::CalculatorSettings& settings) {
  if (ImGui::CollapsingHeader("Requirements", ImGuiTreeNodeFlags_DefaultOpen)) {
    if (ImGui::BeginTable("Requirements", 2, kSettingsTableFlags)) {
      ImGui::TableSetupColumn("Requirement",
                              ImGuiTableColumnFlags_NoHeaderLabel);
      ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_NoHeaderLabel);

      DrawSettingsTableRow("Min Ouput");
      ImGui::InputFloat("##Min Ouput", &settings.min_output, 0, 0, "%.2f");

      DrawSettingsTableRow("Max Ouput");
      ImGui::InputFloat("##Max Ouput", &settings.max_output, 0, 0, "%.2f");

      DrawSettingsTableRow("Clients");
      ImGui::InputInt("##Clients", &settings.num_clients);

      ImGui::EndTable();
    }
  }
}

///
void DrawFamilySettings(std::string_view label,
                        calc::FamilySettings& settings) {
  ImGui::TableNextRow();

  ImGui::TableNextColumn();
  ImGui::Checkbox(label.data(), &settings.enabled);

  ImGui::TableNextColumn();
  ImGui::SetNextItemWidth(-std::numeric_limits<float>::min());
  ImGui::InputFloat(IdLabel(settings.family_id).c_str(), &settings.cost, 0, 0,
                    "%.2f");
}

///
void DrawFamilies(core::Project& project) {
  if (ImGui::CollapsingHeader("Node Cost", ImGuiTreeNodeFlags_DefaultOpen)) {
    if (ImGui::BeginTable("Node Cost", 2, kSettingsTableFlags)) {
      ImGui::TableSetupScrollFreeze(0, 1);
      ImGui::TableSetupColumn("Node Type");
      ImGui::TableSetupColumn("Cost, $");
      ImGui::TableHeadersRow();

      for (auto& settings :
           project.GetSettings().calculator_settings.family_settings) {
        const auto& family =
            core::Project::FindFamily(project, settings.family_id);

        DrawFamilySettings(family.CreateUiTraits()->GetLabel(), settings);
      }

      ImGui::EndTable();
    }
  }
}

void TraverseDepthFirstEx(
    const calc::TreeNode& tree_node,
    const std::invocable<const calc::TreeNode&> auto& visitor_before_children,
    const std::invocable<const calc::TreeNode&> auto& visitor_after_children) {
  visitor_before_children(tree_node);

  for (const auto& child : tree_node.GetChildren()) {
    TraverseDepthFirstEx(child.second, visitor_before_children,
                         visitor_after_children);
  }

  visitor_after_children(tree_node);
}

auto MakeDiagrams(core::Project& project,
                  const std::vector<calc::TreeNode>& roots,
                  int trees_per_diagram) {
  auto diagrams = std::vector<core::Diagram>{};

  const auto num_trees = std::min(static_cast<int>(roots.size()), 1000);

  auto num_diagrams = num_trees / trees_per_diagram;

  if (num_diagrams * trees_per_diagram < num_trees) {
    ++num_diagrams;
  }

  for (auto diagram_index = 0; diagram_index < num_diagrams; ++diagram_index) {
    const auto start_tree_index = diagram_index * trees_per_diagram;
    const auto end_tree_index =
        std::min((diagram_index + 1) * trees_per_diagram, num_trees) - 1;
    auto new_diagram =
        core::Diagram{"Calc trees " + std::to_string(start_tree_index) + "-" +
                      std::to_string(end_tree_index)};

    for (auto i = start_tree_index; i <= end_tree_index; ++i) {
      const auto& root = roots[i];
      auto parent_stack =
          std::stack<std::pair<const calc::TreeNode*, const core::INode*>>{};

      TraverseDepthFirstEx(
          root,
          [&project, &new_diagram, &parent_stack](const auto& tree_node) {
            auto& id_generator = project.GetIdGenerator();

            const auto& family =
                core::Project::FindFamily(project, tree_node.GetFamilyId());
            const auto& node =
                new_diagram.EmplaceNode(family.CreateNode(id_generator));

            if (!parent_stack.empty()) {
              const auto& [parent_tree_node, parent_node] = parent_stack.top();

              for (const auto& child_node : parent_tree_node->GetChildren()) {
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
    }

    diagrams.emplace_back(std::move(new_diagram));
  }

  return diagrams;
}
}  // namespace

///
auto CalculatorView::GetLabel() const -> std::string { return "Calculator"; }

///
void CalculatorView::Draw(coreui::Project& project) {
  auto& core_project = project.GetProject();

  if (calculation_task_.has_value()) {
    if (const auto& result = calculation_task_->GetResult()) {
      for (auto& diagram : MakeDiagrams(core_project, {*result}, 30)) {
        project.AddDiagram(std::move(diagram))
            // .Then([safe_this = safe_owner_.MakeSafe(this)]() {
            //   const auto flow_trees =
            //       flow::BuildFlowTrees(safe_this->diagram_->GetDiagram());
            //   safe_this->diagram_->GetNodeMover().ArrangeAsTrees(flow_trees);
            // })
            // .Then(([]() { ne::NavigateToContent(); }))
            ;
      }

      calculation_task_.reset();
    }
  }

  const auto content_scope = DrawContentScope();

  if (!IsOpened()) {
    return;
  }

  auto& settings = core_project.GetSettings().calculator_settings;
  const auto calculation_running =
      calculation_task_.has_value() && calculation_task_->IsRunning();

  {
    const auto disable_scope = EnableIf(!calculation_running);

    if (ImGui::Button("Calculate")) {
      calculation_task_.emplace(calc::Calculator::ConstructorArgs{
          .settings = settings,
          .input_trees = ToCalcTrees(project.GetDiagram().GetDiagram()),
          .client_node = calc::TreeNode{GetClientFamilyId(core_project)},
          .family_nodes = AsFamilyNodes(core_project.GetFamilies(),
                                        settings.family_settings)});
    }
  }

  ImGui::SameLine();

  {
    const auto disable_scope = EnableIf(calculation_running);

    if (ImGui::Button("Cancel")) {
      Expects(calculation_task_.has_value());
      calculation_task_->Stop();
    }
  }

  DrawProgressBar();
  DrawRequirements(settings);
  DrawFamilies(core_project);
}

///
void CalculatorView::DrawProgressBar() const {
  if (!calculation_task_.has_value()) {
    return;
  }

  ImGui::SameLine();

  const auto progress = calculation_task_->GetProgress();
  const auto label = std::to_string(static_cast<int>(progress * 100)) + "%";

  ImGui::ProgressBar(progress, {-std::numeric_limits<float>::min(), 0},
                     label.c_str());
}
}  // namespace vh::ponc::draw