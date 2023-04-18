#ifndef VH_PONC_CALC_TREE_NODE_H_
#define VH_PONC_CALC_TREE_NODE_H_

#include <map>

#include "core_i_family.h"

namespace vh::ponc::calc {
///
using Cost = int;
///
using FlowValue = int;
///
using NumClients = int;
///
using OutputIndex = int;

///
class TreeNode {
 public:
  ///
  TreeNode() = default;
  ///
  explicit TreeNode(core::FamilyId family_id,
                    std::vector<FlowValue> outputs = {}, Cost node_cost = {});

  ///
  auto GetFamilyId() const -> core::FamilyId;
  ///
  auto GetOutputs() const -> const std::vector<FlowValue> &;
  ///
  auto GetNodeCost() const -> Cost;
  ///
  auto GetTreeCost() const -> Cost;
  ///
  void SetTreeCost(Cost cost);
  ///
  auto GetInput() const -> FlowValue;
  ///
  auto GetChildren() const -> const std::map<OutputIndex, TreeNode> &;
  ///
  auto EmplaceChild(OutputIndex index, TreeNode child) -> TreeNode &;
  ///
  void EraseChild(OutputIndex index);
  ///
  auto GetNumClients() const -> NumClients;
  ///
  void SetNumClients(NumClients num_clients);

 private:
  ///
  core::FamilyId family_id_{};
  ///
  std::vector<OutputIndex> outputs_{};
  ///
  Cost node_cost_{};
  ///
  NumClients num_clients_{};
  ///
  Cost tree_cost_{};
  ///
  FlowValue input_{};
  ///
  std::map<OutputIndex, TreeNode> child_nodes_{};
};
}  // namespace vh::ponc::calc

#endif  // VH_PONC_CALC_TREE_NODE_H_
