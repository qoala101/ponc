#ifndef VH_PONC_CALC_CALCULATOR_H_
#define VH_PONC_CALC_CALCULATOR_H_

#include <atomic>
#include <cstdint>
#include <functional>
#include <map>
#include <set>
#include <stack>
#include <unordered_map>
#include <unordered_set>

#include "calc_tree_node.h"
#include "core_settings.h"

namespace vh::ponc::calc {
///
class Calculator {
 public:
  ///
  enum class StepStatus { kContinueToNextStep, kStopCalculation };

  ///
  struct ConstructorArgs {
    ///
    core::CalculatorSettings settings{};
    ///
    std::vector<TreeNode> input_nodes{};
    ///
    TreeNode client_node{};
    ///
    std::vector<TreeNode> family_nodes{};
    ///
    std::function<auto(const Calculator &)->StepStatus> step_callback{};
  };

  ///
  explicit Calculator(const ConstructorArgs &args);

  ///
  auto GetProgress() const -> float;
  ///
  auto TakeResult() -> std::vector<TreeNode>;

 private:
  ///
  auto IsOutputInRange(FlowValue ouput) const;
  ///
  auto IsStopped();
  ///
  void FindUniqueOutputs();
  ///
  void FindBestOutputTrees();
  ///
  void FindBestTreesForOutput(FlowValue output, const TreeNode &family_node);
  ///
  auto GetBestTree(FlowValue output, NumClients num_clients)
      -> std::optional<TreeNode *>;
  ///
  auto TestBestTreesPermutation(
      FlowValue output, const TreeNode &family_node,
      std::vector<std::optional<const TreeNode *>> &permutation,
      OutputIndex output_index);
  ///
  void MakeBestTreesPermutation(
      FlowValue output, const TreeNode &family_node,
      std::vector<std::optional<const TreeNode *>> &permutation,
      OutputIndex output_index,
      std::unordered_map<FlowValue, NumClientsIndex> num_clients_indices);
  ///
  void FindBestRootTree();

  ///
  FlowValue min_output_{};
  ///
  FlowValue max_output_{};
  ///
  NumClients num_clients_{};
  ///
  std::vector<TreeNode> input_nodes_{};
  ///
  TreeNode client_node_{};
  ///
  std::vector<TreeNode> family_nodes_{};
  ///
  std::function<auto(const Calculator &)->StepStatus> step_callback_{};
  ///
  std::set<FlowValue> unique_outputs_{};
  ///
  std::map<FlowValue, std::map<NumClients, TreeNode>> best_trees_{};
};
}  // namespace vh::ponc::calc

#endif  // VH_PONC_CALC_CALCULATOR_H_
