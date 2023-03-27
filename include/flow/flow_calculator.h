#ifndef VH_PONC_FLOW_CALCULATOR_H_
#define VH_PONC_FLOW_CALCULATOR_H_

#include <cstdint>
#include <unordered_map>

#include "core_i_family.h"
#include "flow_node_flow.h"
#include "flow_tree.h"

namespace vh::ponc::flow {
struct InputRange {
  float min{};
  float max{};
};

struct FamilyFlow {
  NodeFlow node_flow{};
  int cost{};
};

struct CalculatorInput {
  TreeNode input_tree{};
  NodeFlows input_flows{};
  std::unordered_map<uintptr_t, FamilyFlow> family_flows{};
  std::unordered_map<uintptr_t, InputRange> node_input_ranges{};
};

struct Diagram {
  FlowTree flow_tree{};
  std::unordered_map<uintptr_t, core::FamilyId> node_families{};
};

auto Calculate(const CalculatorInput &input) -> std::vector<Diagram>;
}  // namespace vh::ponc::flow

#endif  // VH_PONC_FLOW_CALCULATOR_H_
