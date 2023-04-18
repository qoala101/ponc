#include "calc_tree_node.h"

#include "cpp_assert.h"

namespace vh::ponc::calc {
///
TreeNode::TreeNode(core::FamilyId family_id, std::vector<OutputIndex> outputs,
                   Cost node_cost)
    : family_id_{family_id},
      outputs_{std::move(outputs)},
      node_cost_{node_cost} {}

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
void TreeNode::SetTreeCost(Cost cost) { tree_cost_ = cost; }

///
auto TreeNode::GetInput() const -> FlowValue { return input_; }

///
auto TreeNode::GetChildren() const -> const std::map<OutputIndex, TreeNode> & {
  return child_nodes_;
}

///
auto TreeNode::EmplaceChild(OutputIndex index, TreeNode child) -> TreeNode & {
  Expects(index >= 0);
  Expects(index < static_cast<OutputIndex>(outputs_.size()));

  EraseChild(index);

  child.input_ = outputs_[index];

  for (auto &child_output : child.outputs_) {
    child_output += child.input_;
  }

  return child_nodes_.emplace(index, std::move(child)).first->second;
}

///
void TreeNode::EraseChild(OutputIndex index) { child_nodes_.erase(index); }

///
auto TreeNode::GetNumClients() const -> NumClients { return num_clients_; }

///
void TreeNode::SetNumClients(NumClients num_clients) {
  num_clients_ = num_clients;
}
}  // namespace vh::ponc::calc