#include "draw_calculator_view.h"

#include <algorithm>
#include <chrono>
#include <concepts>
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
#include "coreui_cloner.h"
#include "coreui_i_family_traits.h"
#include "cpp_assert.h"
#include "draw_disable_if.h"
#include "draw_id_label.h"
#include "draw_settings_table_row.h"
#include "draw_table_flags.h"
#include "flow_algorithms.h"
#include "flow_node_flow.h"
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
void TraverseFreeOutputs(const core::Diagram& diagram,
                         const std::invocable<flow::PinFlow> auto& visitor,
                         const flow::TreeNode& flow_tree,
                         const flow::NodeFlows& node_flows) {
  flow::TraverseDepthFirst(
      flow_tree,
      [&diagram, &visitor, &node_flows](const auto& flow_node) {
        const core::INode& node =
            core::Diagram::FindNode(diagram, flow_node.node_id);
        const auto output_pins = node.GetOutputPinIds();
        const auto output_pin_flows = node.GetInitialFlow().output_pin_flows;

        const auto node_id = flow_node.node_id.Get();
        Expects(node_flows.contains(node_id));
        const auto& node_flow = node_flows.at(node_id);

        for (const auto pin_id : output_pins) {
          const auto pin_id_value = pin_id.Get();

          if (flow_node.child_nodes.contains(pin_id_value)) {
            continue;
          }

          const auto pin_flow = node_flow.output_pin_flows.find(pin_id_value);
          Expects(pin_flow != node_flow.output_pin_flows.cend());
          visitor(*pin_flow);
        }
      },
      [](const auto&) {});
}

///
void TraverseFreeOutputs(const core::Diagram& diagram,
                         const std::invocable<flow::PinFlow> auto& visitor) {
  const auto flow_trees = flow::BuildFlowTrees(diagram);
  const auto node_flows =
      flow::CalculateNodeFlows(flow_trees, [&diagram](const auto node_id) {
        return core::Diagram::FindNode(diagram, node_id).GetInitialFlow();
      });

  for (const auto& flow_tree : flow_trees) {
    TraverseFreeOutputs(diagram, visitor, flow_tree, node_flows);
  }
}

///
auto GetFreeOutputs(const core::Diagram& diagram) {
  auto free_outputs = std::vector<float>{};

  TraverseFreeOutputs(diagram, [&free_outputs](const auto& pin_flow) {
    free_outputs.emplace_back(pin_flow.second);
  });

  return free_outputs;
}

///
auto GetChildOutputIndex(const calc::TreeNode& parent,
                         const calc::TreeNode* child) {
  const auto& parent_children = parent.GetChildren();
  const auto child_node = std::find_if(
      parent_children.cbegin(), parent_children.cend(),
      [child](const auto& child_node) { return &child_node.second == child; });

  Expects(child_node != parent_children.cend());
  return child_node->first;
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
}  // namespace

///
auto CalculatorView::GetLabel() const -> std::string { return "Calculator"; }

///
void CalculatorView::Draw(coreui::Project& project) {
  auto& core_project = project.GetProject();

  if (calculation_task_.has_value()) {
    if (const auto& result = calculation_task_->GetResult()) {
      Expects(diagram_copy_.has_value());
      PopulateDiagram(*result, core_project);
      project.AddDiagram(std::move(*diagram_copy_))
          // .Then([safe_this = safe_owner_.MakeSafe(this)]() {
          //   const auto flow_trees =
          //       flow::BuildFlowTrees(safe_this->diagram_->GetDiagram());
          //   safe_this->diagram_->GetNodeMover().ArrangeAsTrees(flow_trees);
          // })
          // .Then(([]() { ne::NavigateToContent(); }))
          ;

      diagram_copy_.reset();
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
      const auto& diagram = project.GetDiagram().GetDiagram();
      diagram_copy_.emplace(coreui::Cloner::Clone(diagram, core_project));

      calculation_task_.emplace(calc::Calculator::ConstructorArgs{
          .settings = settings,
          .input_node = calc::TreeNode{{}, GetFreeOutputs(diagram)},
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
void CalculatorView::PopulateOutput(const calc::TreeNode& output_tree,
                                    ne::PinId output_pin,
                                    core::Project& project) {
  auto& id_generator = project.GetIdGenerator();
  auto parent_stack =
      std::stack<std::pair<const calc::TreeNode*, const core::INode*>>{};

  TraverseDepthFirstEx(
      output_tree,
      [output_pin, &project, &id_generator, &parent_stack,
       &diagram_copy = diagram_copy_](const auto& tree_node) {
        const auto& family =
            core::Project::FindFamily(project, tree_node.GetFamilyId());
        const auto& node =
            diagram_copy->EmplaceNode(family.CreateNode(id_generator));

        auto parent_output_pin = output_pin;

        if (!parent_stack.empty()) {
          const auto& [parent_tree_node, parent_node] = parent_stack.top();
          const auto parent_output_pins = parent_node->GetOutputPinIds();
          const auto output_index =
              GetChildOutputIndex(*parent_tree_node, &tree_node);

          Expects(output_index < static_cast<int>(parent_output_pins.size()));
          parent_output_pin = parent_output_pins[output_index];
        }

        const auto input_pin = node.GetInputPinId();
        Expects(input_pin.has_value());

        const auto link = core::Link{.id = id_generator.Generate<ne::LinkId>(),
                                     .start_pin_id = parent_output_pin,
                                     .end_pin_id = *input_pin};

        diagram_copy->EmplaceLink(link);

        parent_stack.emplace(&tree_node, &node);
      },
      [&parent_stack](const auto&) { parent_stack.pop(); });
}

///
void CalculatorView::PopulateDiagram(const calc::TreeNode& calculated_tree,
                                     core::Project& project) {
  Expects(diagram_copy_.has_value());
  coreui::Cloner::RewireIds(*diagram_copy_, project);

  auto output_index = 0;

  TraverseFreeOutputs(*diagram_copy_, [this, &calculated_tree, &project,
                                       &output_index](const auto& pin_flow) {
    const auto& output_trees = calculated_tree.GetChildren();
    const auto output_tree = output_trees.find(output_index);

    if (output_tree != output_trees.cend()) {
      PopulateOutput(output_tree->second, pin_flow.first, project);
    }

    ++output_index;
  });
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