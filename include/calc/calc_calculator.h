#ifndef VH_PONC_CALC_CALCULATOR_H_
#define VH_PONC_CALC_CALCULATOR_H_

#include <atomic>
#include <cstdint>
#include <functional>
#include <map>
#include <mutex>
#include <set>
#include <stack>
#include <unordered_map>
#include <unordered_set>

#include "calc_settings.h"
#include "calc_tree_node.h"
#include "core_i_family.h"
#include "core_id_value.h"

namespace vh::ponc::calc {
///
class Calculator {
 public:
  ///
  enum class StepStatus { kContinueToNextStep, kStopCalculation };

  ///
  struct ConstructorArgs {
    ///
    CalculatorSettings settings{};
    ///
    core::FamilyId input_family_id{};
    ///
    core::FamilyId client_family_id{};
    ///
    std::unordered_map<core::IdValue<core::FamilyId>, std::vector<float>>
        family_outputs{};
    ///
    std::function<auto(const Calculator &)->StepStatus> step_callback{};
  };

  ///
  explicit Calculator(const ConstructorArgs &args);

  ///
  auto GetProgress() const -> float;
  ///
  auto GetLastResult() const -> const TreeNode &;

 private:
  ///
  using TreeIndex = int;

  struct RecursionState {
    ///
    TreeNode node{};
    ///
    bool node_has_same_valid_outputs{};
  };

  ///
  auto IsOutputInRange(FlowValue ouput) const;
  ///
  auto IsStopped();
  ///
  void TraverseNode(TreeNode &node);
  ///
  void VisitOutput(TreeNode &node, OutputIndex output_index);
  ///
  void FindBestChildPermutationForNode(const TreeNode &node);
  ///
  void MakeNextChildPermutation(RecursionState &state, OutputIndex output_index,
                                TreeIndex prev_output_tree_index);
  ///
  void TestChildPermutation(const TreeNode &node);
  ///
  void RecordChildPermutation(const TreeNode &node);

  ///
  FlowValue min_output_{};
  ///
  FlowValue max_output_{};
  ///
  NumClients num_clients_{};
  ///
  TreeNode input_node_;
  ///
  TreeNode client_node_;
  ///
  std::vector<TreeNode> family_nodes_{};
  ///
  std::function<auto(const Calculator &)->StepStatus> step_callback_{};
  ///
  std::unordered_set<OutputIndex> visited_outputs_{};
  ///
  std::map<FlowValue, std::map<NumClients, TreeNode>>
      best_tree_per_num_clients_per_output_{};
};
}  // namespace vh::ponc::calc

#endif  // VH_PONC_CALC_CALCULATOR_H_
