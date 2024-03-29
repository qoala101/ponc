/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "coreui_calculator.h"

#include <imgui_node_editor.h>

#include <algorithm>
#include <concepts>
#include <iterator>
#include <numeric>
#include <optional>
#include <sstream>
#include <stack>
#include <string>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

#include "calc_calculator.h"
#include "calc_resolution.h"
#include "calc_tree_node.h"
#include "calc_tree_traversal.h"
#include "calc_types.h"
#include "core_diagram.h"
#include "core_i_node.h"
#include "core_id_generator.h"
#include "core_link.h"
#include "core_project.h"
#include "core_settings.h"
#include "coreui_cloner.h"
#include "coreui_diagram.h"
#include "coreui_event.h"
#include "coreui_log.h"
#include "coreui_native_facade.h"
#include "coreui_node_mover.h"
#include "coreui_project.h"
#include "cpp_assert.h"
#include "cpp_scope.h"
#include "flow_algorithms.h"
#include "flow_node_flow.h"
#include "flow_tree_node.h"
#include "flow_tree_traversal.h"

namespace vh::ponc::coreui {
namespace {
///
auto GetNodeOutputs(const core::INode& node) {
  const auto& output_pins = node.GetOutputPinIds();

  if (output_pins.empty()) {
    return std::vector<float>{};
  }

  const auto output_pin_flows = node.GetInitialFlow().output_pin_flows;

  auto outputs = std::vector<float>{};
  outputs.reserve(output_pins.size());

  std::transform(output_pins.cbegin(), output_pins.cend(),
                 std::back_inserter(outputs),
                 [&output_pin_flows](const auto pin_id) {
                   const auto pin_id_value = pin_id.Get();
                   Expects(output_pin_flows.contains(pin_id_value));
                   return output_pin_flows.at(pin_id_value);
                 });

  return outputs;
}

///
void TraverseFreeOutputs(
    const core::Diagram& diagram,
    const std::invocable<const flow::TreeNode&, flow::PinFlow> auto& visitor,
    const flow::TreeNode& flow_tree, const flow::NodeFlows& node_flows) {
  flow::TraverseDepthFirst(
      flow_tree,
      [&diagram, &visitor, &node_flows](const auto& tree_node) {
        const core::INode& node =
            core::Diagram::FindNode(diagram, tree_node.node_id);
        const auto output_pins = node.GetOutputPinIds();
        const auto output_pin_flows = node.GetInitialFlow().output_pin_flows;

        const auto node_id = tree_node.node_id.Get();
        Expects(node_flows.contains(node_id));
        const auto& node_flow = node_flows.at(node_id);

        for (const auto pin_id : output_pins) {
          const auto pin_id_value = pin_id.Get();

          if (tree_node.child_nodes.contains(pin_id_value)) {
            continue;
          }

          const auto pin_flow = node_flow.output_pin_flows.find(pin_id_value);
          Expects(pin_flow != node_flow.output_pin_flows.cend());

          visitor(tree_node, *pin_flow);
        }
      },
      [](const auto&) {});
}

///
void TraverseFreeOutputs(
    const core::Diagram& diagram, const core::Project& project,
    const std::invocable<const flow::TreeNode&, flow::PinFlow> auto& visitor) {
  const auto flow_trees = flow::BuildFlowTrees(diagram);
  const auto node_flows = flow::CalculateNodeFlows(
      flow_trees,
      [&diagram](const auto node_id) {
        return core::Diagram::FindNode(diagram, node_id).GetInitialFlow();
      },
      [&diagram, &project](const auto pin_id) {
        const auto link = core::Diagram::FindPinLink(diagram, pin_id);
        Expects(link.has_value());
        return core::Link::GetDrop(**link, project);
      });

  for (const auto& flow_tree : flow_trees) {
    TraverseFreeOutputs(diagram, visitor, flow_tree, node_flows);
  }
}

///
auto GetFreeOutputs(const core::Diagram& diagram,
                    const core::Project& project) {
  auto free_outputs = std::vector<std::vector<float>>{};
  auto traversing_node_id = std::optional<ne::NodeId>{};

  TraverseFreeOutputs(diagram, project,
                      [&free_outputs, &traversing_node_id](
                          const auto& tree_node, const auto& pin_flow) {
                        if (!traversing_node_id.has_value() ||
                            (*traversing_node_id != tree_node.node_id)) {
                          traversing_node_id = tree_node.node_id;
                          free_outputs.emplace_back();
                        }

                        Expects(!free_outputs.empty());
                        free_outputs.back().emplace_back(pin_flow.second);
                      });

  return free_outputs;
}

///
auto GetInputNodes(const core::Diagram& diagram, const core::Project& project) {
  const auto free_outputs = GetFreeOutputs(diagram, project);

  auto input_nodes = std::vector<calc::TreeNode>{};
  input_nodes.reserve(free_outputs.size());

  std::transform(free_outputs.cbegin(), free_outputs.cend(),
                 std::back_inserter(input_nodes), [](const auto& free_outputs) {
                   return calc::TreeNode{
                       .outputs = calc::ToCalculatorResolution(free_outputs)};
                 });

  return input_nodes;
}

///
auto GetClientFamilyId(const core::Project& project) {
  const auto& families = project.GetFamilies();
  const auto client_family =
      std::find_if(families.cbegin(), families.cend(), [](const auto& family) {
        const auto& type = family->GetType();
        return type.has_value() && (*type == core::FamilyType::kClient);
      });

  Expects(client_family != families.cend());
  return (*client_family)->GetId();
}

///
auto GetChildOutputIndex(const calc::TreeNode& parent,
                         const calc::TreeNode* child) {
  const auto child_node = std::find_if(
      parent.child_nodes.cbegin(), parent.child_nodes.cend(),
      [child](const auto& child_node) { return &child_node.second == child; });

  Expects(child_node != parent.child_nodes.cend());
  return child_node->first;
}

///
auto GetOutputTrees(
    const std::vector<flow::TreeNode>& flow_trees,
    const std::map<core::IdValue<ne::PinId>, core::IdValue<ne::NodeId>>&
        output_root_ids) {
  auto output_trees = std::vector<flow::TreeNode>{};
  output_trees.reserve(output_root_ids.size());

  std::transform(output_root_ids.cbegin(), output_root_ids.cend(),
                 std::back_inserter(output_trees),
                 [&flow_trees](const auto& output_root) {
                   return flow::FindTreeNode(flow_trees, output_root.second);
                 });

  return output_trees;
}

///
auto GetFirstLevelChild(const std::vector<calc::TreeNode>& tree_nodes,
                        int output_index)
    -> std::optional<const calc::TreeNode*> {
  auto delta = 0;

  for (const auto& tree_node : tree_nodes) {
    const auto next_delta = delta + static_cast<int>(tree_node.outputs.size());

    if (output_index < next_delta) {
      const auto child_index = output_index - delta;
      const auto child_tree = tree_node.child_nodes.find(child_index);

      if (child_tree == tree_node.child_nodes.cend()) {
        return std::nullopt;
      }

      return &child_tree->second;
    }

    delta = next_delta;
  }

  Expects(false);
}
}  // namespace

///
Calculator::Calculator(cpp::SafePtr<Project> parent_project)
    : parent_project_{std::move(parent_project)} {}

///
auto Calculator::PopulateOutput(const calc::TreeNode& output_tree,
                                ne::PinId output_pin) {
  auto& project = parent_project_->GetProject();
  auto& id_generator = project.GetIdGenerator();

  auto parent_stack =
      std::stack<std::pair<const calc::TreeNode*, const core::INode*>>{};
  auto output_root_id = ne::NodeId{};

  calc::TraverseDepthFirst(
      output_tree,
      [output_pin, &project, &id_generator, &parent_stack, &output_root_id,
       &diagram_copy = diagram_copy_](const auto& tree_node) {
        const auto& family =
            core::Project::FindFamily(project, tree_node.family_id);
        const auto& node =
            diagram_copy->EmplaceNode(family.CreateNode(id_generator));

        auto parent_output_pin = output_pin;

        if (parent_stack.empty()) {
          output_root_id = node.GetId();
        } else {
          const auto& [parent_tree_node, parent_node] = parent_stack.top();
          const auto parent_output_pins = parent_node->GetOutputPinIds();
          const auto output_index =
              GetChildOutputIndex(*parent_tree_node, &tree_node);

          Expects(output_index < static_cast<int>(parent_output_pins.size()));
          parent_output_pin = parent_output_pins[output_index];
        }

        const auto input_pin =
            core::INode::GetFirstPinOfKind(node, ne::PinKind::Input);
        const auto link = core::Link{.id = id_generator.Generate<ne::LinkId>(),
                                     .start_pin_id = parent_output_pin,
                                     .end_pin_id = input_pin};

        diagram_copy->EmplaceLink(link);

        parent_stack.emplace(&tree_node, &node);
      },
      [&parent_stack](const auto&) { parent_stack.pop(); });

  return output_root_id;
}

///
auto Calculator::PopulateDiagram(
    const std::vector<calc::TreeNode>& calculated_trees) {
  Expects(diagram_copy_.has_value());

  auto& core_project = parent_project_->GetProject();
  coreui::Cloner::RewireIds(core::Diagram::GetIds(*diagram_copy_),
                            core_project.GetIdGenerator());

  auto output_root_ids =
      std::map<core::IdValue<ne::PinId>, core::IdValue<ne::NodeId>>{};
  auto output_index = 0;

  TraverseFreeOutputs(
      *diagram_copy_, core_project,
      [this, &calculated_trees, &output_root_ids, &output_index](
          const auto&, const auto& pin_flow) {
        if (const auto output_tree =
                GetFirstLevelChild(calculated_trees, output_index)) {
          const auto output_root_id =
              PopulateOutput(**output_tree, pin_flow.first);
          output_root_ids.emplace(pin_flow.first, output_root_id);
        }

        ++output_index;
      });

  return output_root_ids;
}

///
void Calculator::OnFrame() {
  if (!calculation_task_.has_value()) {
    return;
  }

  const auto result = calculation_task_->GetResult();

  if (!result.has_value()) {
    return;
  }

  ProcessResult(*result);
}

///
auto Calculator::ValidateInputs(
    const std::vector<calc::TreeNode>& input_nodes) const {
  if (input_nodes.empty()) {
    parent_project_->GetLog().Write(
        LogLevel::kError, "Calculator: Diagram should have free outputs.");
    return false;
  }

  const auto& settings =
      parent_project_->GetProject().GetSettings().calculator_settings;

  if (settings.min_output > settings.max_output) {
    parent_project_->GetLog().Write(
        LogLevel::kError, "Calculator: Min Output should be <= Max Output.");
    return false;
  }

  return true;
}

///
auto Calculator::AsFamilyNodes(
    const std::vector<std::unique_ptr<core::IFamily>>& families) const {
  const auto& family_settings = parent_project_->GetProject()
                                    .GetSettings()
                                    .calculator_settings.family_settings;

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

    const auto cost = calc::ToCalculatorResolution(settings.cost);
    const auto sample_node = (*family)->CreateSampleNode();
    const auto outputs = GetNodeOutputs(*sample_node);

    family_nodes.emplace_back(
        calc::TreeNode{.family_id = settings.family_id,
                       .node_cost = cost,
                       .tree_cost = cost,
                       .outputs = calc::ToCalculatorResolution(outputs)});
  }

  return family_nodes;
}

///
void Calculator::Calculate() {
  const auto& diagram = parent_project_->GetDiagram().GetDiagram();
  auto& core_project = parent_project_->GetProject();
  auto input_nodes = GetInputNodes(diagram, core_project);

  if (!ValidateInputs(input_nodes)) {
    return;
  }

  const auto& families = core_project.GetFamilies();

  diagram_copy_.emplace(coreui::Cloner::Clone(diagram, families));

  calculation_task_.emplace(calc::Calculator::ConstructorArgs{
      .settings = core_project.GetSettings().calculator_settings,
      .input_nodes = std::move(input_nodes),
      .client_node =
          calc::TreeNode{.family_id = GetClientFamilyId(core_project),
                         .num_clients = 1},
      .family_nodes = AsFamilyNodes(families)});
}

///
void Calculator::Cancel() {
  diagram_copy_.reset();
  calculation_task_.reset();
}

///
auto Calculator::IsRunning() const -> bool {
  return calculation_task_.has_value() && calculation_task_->IsRunning();
}

///
auto Calculator::GetProgress() const -> float {
  Expects(calculation_task_.has_value());
  return calculation_task_->GetProgress();
}

///
void Calculator::LogResult(const std::vector<calc::TreeNode>& calculated_trees,
                           std::string_view diagram_name) const {
  const auto total_cost = calc::FromCalculatorResolution(
      std::accumulate(calculated_trees.cbegin(), calculated_trees.cend(), 0,
                      [](const auto total_cost, const auto& tree) {
                        return total_cost + tree.tree_cost;
                      }));

  const auto num_clients =
      std::accumulate(calculated_trees.cbegin(), calculated_trees.cend(), 0,
                      [](const auto num_clients, const auto& tree) {
                        return num_clients + tree.num_clients;
                      });

  auto log_stream = std::ostringstream{};
  log_stream << "Calculator: Added " << num_clients << " clients for "
             << total_cost << "$ (" << diagram_name.data() << ")";

  parent_project_->GetLog().Write(LogLevel::kDone, log_stream.str());
}

///
auto Calculator::ValidateResult(
    const std::map<core::IdValue<ne::PinId>, core::IdValue<ne::NodeId>>&
        output_root_ids) const {
  if (output_root_ids.empty()) {
    parent_project_->GetLog().Write(
        LogLevel::kDone, "Calculator: Couldn't fit any more clients.");
    return false;
  }

  return true;
}

///
void Calculator::ProcessResult(
    const std::vector<calc::TreeNode>& calculated_trees) {
  const auto reset_scope = cpp::Scope{[this]() {
    diagram_copy_.reset();
    calculation_task_.reset();
  }};

  const auto output_root_ids = PopulateDiagram(calculated_trees);

  if (!ValidateResult(output_root_ids)) {
    return;
  }

  Expects(diagram_copy_.has_value());
  auto flow_trees = flow::BuildFlowTrees(*diagram_copy_);
  auto output_trees = GetOutputTrees(flow_trees, output_root_ids);

  const auto& diagrams = parent_project_->GetProject().GetDiagrams();
  auto new_diagram_name = core::Diagram::MakeUniqueDiagramName(
      diagrams, diagram_copy_->GetName(), "calc.");

  LogResult(calculated_trees, new_diagram_name);
  diagram_copy_->SetName(std::move(new_diagram_name));

  parent_project_->AddDiagram(std::move(*diagram_copy_))
      .Then([parent_project = parent_project_, output_trees,
             flow_trees = std::move(flow_trees)]() {
        auto& node_mover = parent_project->GetDiagram().GetNodeMover();
        node_mover.ArrangeAsNewTrees(output_trees);
      })
      .Then([parent_project = parent_project_,
             output_trees = std::move(output_trees)]() {
        for (const auto& tree_node : output_trees) {
          flow::TraverseDepthFirst(
              tree_node,
              [](const auto& tree_node) {
                NativeFacade::SelectNode(tree_node.node_id, true);
              },
              [](const auto&) {});
        }

        ne::NavigateToSelection();
      });
}
}  // namespace vh::ponc::coreui