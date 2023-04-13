#ifndef VH_PONC_FLOW_CALCULATOR_H_
#define VH_PONC_FLOW_CALCULATOR_H_

#include <cstdint>
#include <map>
#include <set>
#include <stack>
#include <unordered_map>

#include "core_i_family.h"

namespace vh::ponc::flow {
template <typename T>
struct Range {
  T min{};
  T max{};
};

template <typename T>
struct Family {
  core::FamilyId family_id{};
  std::vector<T> outputs{};
  T cost{};
};

template <typename T>
struct CalculatorInput {
  core::FamilyId input_family_id{};
  core::FamilyId output_family_id{};
  T input{};
  Range<T> output_range{};
  int num_outputs{};
  std::vector<Family<T>> family_flows{};
};

struct TreeNodeEx {
  static auto FromFamily(const Family<int> &family_flow) -> TreeNodeEx;

  auto EmplaceChild(int index, TreeNodeEx child) -> TreeNodeEx &;
  void EraseChild(int index);

  auto CalculateCost() const -> int;

  core::FamilyId family_id{};
  std::vector<int> outputs{};

  auto GetNodeCost() const -> int;
  auto GetChildren() const -> const std::map<int, TreeNodeEx> &;

  int input{};

 private:
  std::map<int, TreeNodeEx> child_nodes{};
  int node_cost_{};

  friend auto operator==(const TreeNodeEx &, const TreeNodeEx &)
      -> bool = default;
};

class Calculator {
 public:
  auto Start(const CalculatorInput<int> &data) -> std::vector<TreeNodeEx>;
  auto Step() -> bool;
  void Stop();
  auto GetResult() -> TreeNodeEx;

 private:
  void MakeCombination(
      const TreeNodeEx &node,
      std::map<int /*ouput_index*/, std::pair<int, TreeNodeEx>> &combination,
      int output_index, int start_with_combination, int clients_sum, int clients_cost,
      bool same_outputs);

  void RememberAlgStepSimple(const std::vector<Family<int>> &family_flows,
                             const TreeNodeEx &root, TreeNodeEx &node);

  CalculatorInput<int> data_{};

  std::vector<TreeNodeEx> out_trees{};
  TreeNodeEx client_node_{};

  std::set<int> visited_outputs{};

  std::map<int, std::map<int, TreeNodeEx>> output_tree_per_num_clients{};
};
}  // namespace vh::ponc::flow

#endif  // VH_PONC_FLOW_CALCULATOR_H_
