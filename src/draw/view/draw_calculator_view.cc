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
void DrawFamily(const core::IFamily& family, flow::FamilyFlow& family_flow) {
  ImGui::TableNextRow();

  ImGui::TableNextColumn();
  ImGui::TextUnformatted(family.CreateUiTraits()->GetLabel().c_str());

  ImGui::TableNextColumn();
  ImGui::SetNextItemWidth(-std::numeric_limits<float>::min());
  ImGui::InputFloat(IdLabel(family.GetId()).c_str(), &family_flow.cost);
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

auto MakeDiagram(core::Project& project, const flow::TreeNodeEx& diagram) {
  auto new_diagram = core::Diagram{"new diag"};
  auto parent_stack =
      std::stack<std::pair<const flow::TreeNodeEx*, const core::INode*>>{};

  TraverseDepthFirst(
      diagram,
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

auto MakeDiagrams(core::Project& project,
                  const std::vector<flow::TreeNodeEx>& roots,
                  int trees_per_diagram) {
  auto diagrams = std::vector<core::Diagram>{};

  const auto num_trees = std::min(static_cast<int>(roots.size()), 100);

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
          std::stack<std::pair<const flow::TreeNodeEx*, const core::INode*>>{};

      TraverseDepthFirst(
          root,
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
    }

    diagrams.emplace_back(std::move(new_diagram));
  }

  return diagrams;
}
}  // namespace

auto CalculatorView::GetLabel() const -> std::string {
  return "BETA: Calculator";
}

void CalculatorView::Draw(core::Project& project, const Callbacks& callbacks) {
  const auto content_scope = DrawContentScope();

  if (!IsOpened()) {
    return;
  }

  ImGui::SetNextItemWidth(-std::numeric_limits<float>::min());
  ImGui::InputInt("", &num_inputs_);

  if (num_inputs_ <= 0) {
    num_inputs_ = 1;
  }

  if (static_cast<int>(required_inputs_.size()) != num_inputs_) {
    const auto& settings = project.GetSettings();
    required_inputs_.resize(num_inputs_, {.min = settings.low_flow - 6,
                                          .max = settings.high_flow - 6});
  }

  const auto calculate_pressed = ImGui::Button("Calculate");
  static auto SPLIT_BY = 30;

  ImGui::SetNextItemWidth(-std::numeric_limits<float>::min());
  ImGui::InputInt("Split by", &SPLIT_BY);

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

      auto index = 0;

      for (const auto& family : project.GetFamilies()) {
        if (const auto family_is_default = family->GetType().has_value()) {
          continue;
        }

        const auto label = family->CreateUiTraits()->GetLabel();

        if (!label.starts_with("Splitter")) {
          continue;
        }

        if (!label.starts_with("Splitter 1x2") &&
            !label.starts_with("Splitter 1x4")) {
          continue;
        }

        const auto sample_node = family->CreateSampleNode();

        if (!sample_node->GetInputPinId().has_value() ||
            sample_node->GetOutputPinIds().empty()) {
          continue;
        }

        const auto output_pin_flows =
            sample_node->GetInitialFlow().output_pin_flows;

        auto outputs = std::vector<float>{};
        std::transform(output_pin_flows.begin(), output_pin_flows.end(),
                       std::back_inserter(outputs),
                       [](const auto& pair) { return pair.second; });

        const auto family_id = family->GetId().Get();
        auto& family_flow = (family_flows_.size() > index)
                                ? family_flows_[index]
                                : family_flows_.emplace_back(flow::FamilyFlow{
                                      .family_id = family_id,
                                      .outputs = std::move(outputs),
                                      .cost = 10});

        DrawFamily(*family, family_flow);

        ++index;
      }

      ImGui::EndTable();
    }
  }

  if (ImGui::CollapsingHeader("Required Inputs",
                              ImGuiTreeNodeFlags_DefaultOpen)) {
    if (ImGui::BeginTable("Required Inputs", 2, table_flags)) {
      ImGui::TableSetupColumn("Min");
      ImGui::TableSetupColumn("Max");
      ImGui::TableHeadersRow();

      auto index = 0;

      for (auto& required_input : required_inputs_) {
        ImGui::TableNextRow();

        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-std::numeric_limits<float>::min());
        ImGui::InputFloat((IdLabel(index) + "Min").c_str(), &required_input.min,
                          0, 0, "%.3f");

        ImGui::TableNextColumn();
        ImGui::SetNextItemWidth(-std::numeric_limits<float>::min());
        ImGui::InputFloat((IdLabel(index) + "Max").c_str(), &required_input.max,
                          0, 0, "%.3f");

        ++index;
      }

      ImGui::EndTable();
    }
  }

  if (!calculate_pressed) {
    return;
  }

  const auto result = flow::Calculate(
      {.family_flows = family_flows_, .input_ranges = required_inputs_});

  std::cout << "Calculated: " << result.size() << " trees\n";

  callbacks.calculated_diagrams(MakeDiagrams(project, result, SPLIT_BY));
}
}  // namespace vh::ponc::draw