#include "flow_calculator.h"

#include <vector>

#include "core_id_generator.h"
#include "cpp_assert.h"
#include "flow_tree.h"
#include "flow_tree_traversal.h"
#include "imgui_node_editor.h"

namespace vh::ponc::flow {
namespace {
void TraverseDepthFirst(
    const TreeNodeEx &tree_node,
    const std::invocable<const TreeNodeEx &> auto &visitor_before_children,
    const std::invocable<const TreeNodeEx &> auto &visitor_after_children) {
  visitor_before_children(tree_node);

  for (const auto &child : tree_node.child_nodes) {
    TraverseDepthFirst(child.second, visitor_before_children,
                       visitor_after_children);
  }

  visitor_after_children(tree_node);
}
}  // namespace

auto Calculate(const CalculatorInput &input) -> std::vector<Diagram> {
  const auto &input_tree = input.input_tree;
  const auto &family_flows = input.family_flows;
  const auto &node_input_ranges = input.leaf_input_ranges;
  auto id_generator = core::IdGenerator{};

  const auto *leaf = static_cast<const TreeNodeEx *>(nullptr);

  TraverseDepthFirst(
      input_tree,
      [&leaf](const auto &tree_node) {
        if (tree_node.child_nodes.empty()) {
          leaf = &tree_node;
        }
      },
      [](const auto &) {});
  Expects(leaf != nullptr);

  auto diagram = Diagram{};
  diagram.output_tree.family_id = input_tree.family_id;
  auto &root = diagram.output_tree;
  auto *last_added_node = &root;

  for (const auto &family_flow : family_flows) {
    last_added_node =
        &last_added_node->child_nodes
             .emplace(0, TreeNodeEx{.family_id = family_flow.first})
             .first->second;
  }

  last_added_node->child_nodes.emplace(
      0, TreeNodeEx{.family_id = leaf->family_id});

  return {std::move(diagram)};
}
}  // namespace vh::ponc::flow