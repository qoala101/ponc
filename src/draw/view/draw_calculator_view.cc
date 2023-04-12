#include "draw_calculator_view.h"

#include <algorithm>
#include <cstdint>
#include <future>
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
#include "cpp_scope.h"
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
void DrawFamily(const core::IFamily& family,
                std::pair<bool, flow::Family<float>>& family_flow) {
  ImGui::TableNextRow();

  ImGui::TableNextColumn();
  ImGui::Checkbox(family.CreateUiTraits()->GetLabel().c_str(),
                  &family_flow.first);

  ImGui::TableNextColumn();
  ImGui::SetNextItemWidth(-std::numeric_limits<float>::min());
  ImGui::InputFloat(IdLabel(family.GetId()).c_str(), &family_flow.second.cost,
                    0, 0, "%.3f");
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

auto MakeDiagrams(core::Project& project,
                  const std::vector<flow::TreeNodeEx>& roots,
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

  const auto calculate_pressed = ImGui::Button("Calculate");

  ImGui::BeginVertical("pr");
  // Animate a simple progress bar
  static float progress = 0.0f, progress_dir = 1.0f;
  if (true) {
    progress += progress_dir * 0.4f * ImGui::GetIO().DeltaTime;
    if (progress >= +1.1f) {
      progress = +1.1f;
      progress_dir *= -1.0f;
    }
    if (progress <= -0.1f) {
      progress = -0.1f;
      progress_dir *= -1.0f;
    }
  }

  // Typically we would use ImVec2(-1.0f,0.0f) or ImVec2(-FLT_MIN,0.0f) to use
  // all available width, or ImVec2(width,0.0f) for a specified width.
  // ImVec2(0.0f,0.0f) uses ItemWidth.
  ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
  ImGui::SameLine(0.0f, ImGui::GetStyle().ItemInnerSpacing.x);
  ImGui::Text("Progress Bar");

  float progress_saturated = std::clamp(progress, 0.0f, 1.0f);
  char buf[32];
  sprintf(buf, "%d/%d", (int)(progress_saturated * 1753), 1753);
  ImGui::ProgressBar(progress, ImVec2(0.f, 0.f), buf);
  ImGui::EndVertical();

  // NOLINTBEGIN(*-signed-bitwise)
  const auto table_flags =
      ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable |
      ImGuiTableFlags_Hideable | ImGuiTableFlags_ContextMenuInBody |
      ImGuiTableFlags_BordersH | ImGuiTableFlags_BordersOuter;
  // NOLINTEND(*-signed-bitwise)

  if (ImGui::CollapsingHeader("Requirements", ImGuiTreeNodeFlags_DefaultOpen)) {
    if (ImGui::BeginTable("Requirements", 2, table_flags)) {
      ImGui::TableSetupColumn("Requirement",
                              ImGuiTableColumnFlags_NoHeaderLabel);
      ImGui::TableSetupColumn("Value", ImGuiTableColumnFlags_NoHeaderLabel);

      ImGui::TableNextRow();

      ImGui::TableNextColumn();
      ImGui::AlignTextToFramePadding();
      ImGui::TextUnformatted("Input");

      ImGui::TableNextColumn();
      ImGui::SetNextItemWidth(-std::numeric_limits<float>::min());
      ImGui::InputFloat("##Input", &input_, 0, 0, "%.3f");

      ImGui::TableNextRow();

      ImGui::TableNextColumn();
      ImGui::AlignTextToFramePadding();
      ImGui::TextUnformatted("Min Ouput");

      ImGui::TableNextColumn();
      ImGui::SetNextItemWidth(-std::numeric_limits<float>::min());
      ImGui::InputFloat("##Min Ouput", &min_output_, 0, 0, "%.3f");

      ImGui::TableNextRow();

      ImGui::TableNextColumn();
      ImGui::AlignTextToFramePadding();
      ImGui::TextUnformatted("Max Ouput");

      ImGui::TableNextColumn();
      ImGui::SetNextItemWidth(-std::numeric_limits<float>::min());
      ImGui::InputFloat("##Max Ouput", &max_output_, 0, 0, "%.3f");

      ImGui::TableNextRow();

      ImGui::TableNextColumn();
      ImGui::AlignTextToFramePadding();
      ImGui::TextUnformatted("Clients");

      ImGui::TableNextColumn();
      ImGui::SetNextItemWidth(-std::numeric_limits<float>::min());
      ImGui::InputInt("##Clients", &num_clients_);

      ImGui::EndTable();
    }
  }

  if (ImGui::CollapsingHeader("Node Cost", ImGuiTreeNodeFlags_DefaultOpen)) {
    if (ImGui::BeginTable("Node Cost", 2, table_flags)) {
      ImGui::TableSetupScrollFreeze(0, 1);
      ImGui::TableSetupColumn("Node Type");
      ImGui::TableSetupColumn("Cost, $");
      ImGui::TableHeadersRow();

      auto index = 0;

      for (const auto& family : project.GetFamilies()) {
        if (const auto family_is_default = family->GetType().has_value()) {
          continue;
        }

        const auto label = family->CreateUiTraits()->GetLabel();
        const auto sample_node = family->CreateSampleNode();
        const auto& output_pin_ids = sample_node->GetOutputPinIds();

        if (!sample_node->GetInputPinId().has_value() ||
            (output_pin_ids.size() < 2)) {
          continue;
        }

        const auto output_pin_flows =
            sample_node->GetInitialFlow().output_pin_flows;

        auto outputs = std::vector<float>{};
        std::transform(output_pin_ids.begin(), output_pin_ids.end(),
                       std::back_inserter(outputs),
                       [&output_pin_flows](const auto pin_id) {
                         return output_pin_flows.at(pin_id.Get());
                       });

        const auto family_id = family->GetId().Get();
        auto& family_flow =
            (family_flows_.size() > index)
                ? family_flows_[index]
                : family_flows_.emplace_back(
                      true, flow::Family<float>{.family_id = family_id,
                                                .outputs = std::move(outputs),
                                                .cost = 100});

        DrawFamily(*family, family_flow);

        ++index;
      }

      ImGui::EndTable();
    }
  }

  if (!calculate_pressed) {
    return;
  }

  auto family_flows = std::vector<flow::Family<int>>{};

  for (const auto& [checked, family] : family_flows_) {
    if (!checked) {
      continue;
    }

    auto& int_family = family_flows.emplace_back(
        flow::Family<int>{.family_id = family.family_id,
                          .cost = static_cast<int>(family.cost * 100)});

    for (const auto output : family.outputs) {
      int_family.outputs.emplace_back(output * 100);
    }
  }

  const auto result = flow::Calculator{}.Start(flow::CalculatorInput<int>{
      .input_family_id = 1,
      .output_family_id = 2,
      .input = static_cast<int>(input_ * 100),
      .output_range = {.min = static_cast<int>(min_output_ * 100),
                       .max = static_cast<int>(max_output_ * 100)},
      .num_outputs = num_clients_,
      .family_flows = family_flows,
  });

  std::cout << "Calculated: " << result.size() << " trees\n";

  callbacks.calculated_diagrams(MakeDiagrams(project, result, 30));
}
}  // namespace vh::ponc::draw