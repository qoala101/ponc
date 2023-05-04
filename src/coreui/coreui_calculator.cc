#include "coreui_calculator.h"

#include "calc_tree_traversal.h"
#include "coreui_cloner.h"
#include "coreui_project.h"
#include "cpp_assert.h"
#include "flow_algorithms.h"
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
auto AsFamilyNodes(
    const std::vector<std::unique_ptr<core::IFamily>>& families,
    const std::vector<core::CalculatorFamilySettings>& family_settings) {
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
auto GetChildOutputIndex(const calc::TreeNode& parent,
                         const calc::TreeNode* child) {
  const auto& parent_children = parent.GetChildren();
  const auto child_node = std::find_if(
      parent_children.cbegin(), parent_children.cend(),
      [child](const auto& child_node) { return &child_node.second == child; });

  Expects(child_node != parent_children.cend());
  return child_node->first;
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
            core::Project::FindFamily(project, tree_node.GetFamilyId());
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

        const auto input_pin = node.GetInputPinId();
        Expects(input_pin.has_value());

        const auto link = core::Link{.id = id_generator.Generate<ne::LinkId>(),
                                     .start_pin_id = parent_output_pin,
                                     .end_pin_id = *input_pin};

        diagram_copy->EmplaceLink(link);

        parent_stack.emplace(&tree_node, &node);
      },
      [&parent_stack](const auto&) { parent_stack.pop(); });

  return output_root_id;
}

///
auto Calculator::PopulateDiagram(const calc::TreeNode& calculated_tree) {
  Expects(diagram_copy_.has_value());
  coreui::Cloner::RewireIds(*diagram_copy_, parent_project_->GetProject());

  auto output_root_ids =
      std::map<core::IdValue<ne::PinId>, core::IdValue<ne::NodeId>>{};
  auto output_index = 0;

  TraverseFreeOutputs(*diagram_copy_, [this, &calculated_tree, &output_root_ids,
                                       &output_index](const auto& pin_flow) {
    const auto& output_trees = calculated_tree.GetChildren();
    const auto output_tree = output_trees.find(output_index);

    if (output_tree != output_trees.cend()) {
      const auto output_root_id =
          PopulateOutput(output_tree->second, pin_flow.first);

      output_root_ids.emplace(pin_flow.first, output_root_id);
    }

    ++output_index;
  });

  return output_root_ids;
}

auto TEMP_FindOutputTrees(
    const std::vector<flow::TreeNode>& flow_trees,
    const std::map<core::IdValue<ne::PinId>, core::IdValue<ne::NodeId>>&
        output_root_ids) {
  auto output_trees = std::vector<flow::TreeNode>{};
  output_trees.reserve(output_root_ids.size());

  std::transform(output_root_ids.cbegin(), output_root_ids.end(),
                 std::back_inserter(output_trees),
                 [&flow_trees](const auto& output_root) {
                   return flow::FindTreeNode(flow_trees, output_root.second);
                 });

  return output_trees;
}

void Calculator::OnFrame() {
  if (!calculation_task_.has_value()) {
    return;
  }

  const auto& result = calculation_task_->GetResult();

  if (!result.has_value()) {
    return;
  }

  const auto output_root_ids = PopulateDiagram(*result);

  Expects(diagram_copy_.has_value());
  auto flow_trees = flow::BuildFlowTrees(*diagram_copy_);
  auto output_trees = TEMP_FindOutputTrees(flow_trees, output_root_ids);

  Expects(diagram_copy_.has_value());
  parent_project_->AddDiagram(std::move(*diagram_copy_))
      .Then([project = parent_project_, flow_trees = std::move(flow_trees),
             output_trees
             // = std::move(output_trees)
  ]() {
        auto& node_mover = project->GetDiagram().GetNodeMover();
        node_mover.ArrangeAsNewTrees(output_trees);
      })
      .Then([project = parent_project_,
             output_trees = std::move(output_trees)]() {
        for (const auto& tree_node : output_trees) {
          flow::TraverseDepthFirst(
              tree_node,
              [](const auto& tree_node) {
                ne::SelectNode(tree_node.node_id, true);
              },
              [](const auto&) {});
        }

        ne::NavigateToSelection();
      });

  diagram_copy_.reset();
  calculation_task_.reset();
}

///
void Calculator::Calculate() {
  const auto& diagram = parent_project_->GetDiagram().GetDiagram();
  auto& core_project = parent_project_->GetProject();
  diagram_copy_.emplace(coreui::Cloner::Clone(diagram, core_project));

  auto& settings = core_project.GetSettings().calculator_settings;
  calculation_task_.emplace(calc::Calculator::ConstructorArgs{
      .settings = settings,
      .input_node = calc::TreeNode{{}, GetFreeOutputs(diagram)},
      .client_node = calc::TreeNode{GetClientFamilyId(core_project)},
      .family_nodes =
          AsFamilyNodes(core_project.GetFamilies(), settings.family_settings)});
}

///
void Calculator::Cancel() { calculation_task_.reset(); }

///
auto Calculator::IsRunning() const -> bool {
  return calculation_task_.has_value() && calculation_task_->IsRunning();
}

///
auto Calculator::GetProgress() const -> float {
  Expects(calculation_task_.has_value());
  return calculation_task_->GetProgress();
}
}  // namespace vh::ponc::coreui