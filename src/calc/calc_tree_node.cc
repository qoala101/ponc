#include "calc_tree_node.h"

#include <algorithm>

#include "calc_resolution.h"
#include "cpp_assert.h"

namespace vh::ponc::calc {
namespace {
///
auto ToCalculatorResolution(const std::vector<float> &values) {
  auto calc_values = std::vector<int>{};
  calc_values.reserve(values.size());

  std::transform(values.cbegin(), values.cend(),
                 std::back_inserter(calc_values), [](const auto value) {
                   return vh::ponc::calc::ToCalculatorResolution(value);
                 });

  return calc_values;
}
}  // namespace

///
TreeNode::TreeNode(core::FamilyId family_id, const std::vector<float> &outputs,
                   float node_cost)
    : family_id_{family_id},
      outputs_{ToCalculatorResolution(outputs)},
      node_cost_{ToCalculatorResolution(node_cost)},
      tree_cost_{node_cost_} {}

///
auto TreeNode::GetFamilyId() const -> core::FamilyId { return family_id_; }

///
auto TreeNode::GetOutputs() const -> const std::vector<OutputIndex> & {
  return outputs_;
}

///
auto TreeNode::GetNodeCost() const -> Cost { return node_cost_; }

///
auto TreeNode::GetTreeCost() const -> Cost { return tree_cost_; }

///
auto TreeNode::GetInput() const -> FlowValue { return input_; }

///
void TreeNode::SetInput(int input) { input_ = input; }

///
auto TreeNode::GetChildren() const -> const std::map<OutputIndex, TreeNode> & {
  return child_nodes_;
}

///
void TreeNode::ClearChildren() {
  child_nodes_.clear();
  tree_cost_ = node_cost_;
  num_clients_ = 0;
}

///
auto TreeNode::EmplaceChild(OutputIndex index, TreeNode child_node)
    -> TreeNode & {
  Expects(index >= 0);
  Expects(index < static_cast<OutputIndex>(outputs_.size()));

  EraseChild(index);

  child_node.input_ = outputs_[index];

  for (auto &child_output : child_node.outputs_) {
    child_output += child_node.input_;
  }

  tree_cost_ += child_node.tree_cost_;
  num_clients_ += child_node.num_clients_;

  return child_nodes_.emplace(index, std::move(child_node)).first->second;
}

///
void TreeNode::EraseChild(OutputIndex index) {
  const auto child_node = child_nodes_.find(index);

  if (child_node == child_nodes_.end()) {
    return;
  }

  tree_cost_ -= child_node->second.tree_cost_;
  num_clients_ -= child_node->second.num_clients_;

  child_nodes_.erase(index);
}

///
auto TreeNode::GetNumClients() const -> NumClients { return num_clients_; }

///
void TreeNode::SetNumClients(NumClients num_clients) {
  num_clients_ = num_clients;
}
}  // namespace vh::ponc::calc