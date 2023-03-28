#ifndef VH_PONC_FLOW_CALCULATOR_H_
#define VH_PONC_FLOW_CALCULATOR_H_

#include <cstdint>
#include <map>
#include <unordered_map>

#include "core_i_family.h"

namespace vh::ponc::flow {
struct InputRange {
  float min{};
  float max{};
};

struct FamilyFlow {
  std::vector<float> output_pin_flows{};
  int cost{};
};

struct TreeNodeEx {
  core::FamilyId family_id{};
  std::map<int, TreeNodeEx> child_nodes{};
};

struct CalculatorInput {
  TreeNodeEx input_tree{};
  std::unordered_map<uintptr_t, FamilyFlow> family_flows{};
  std::unordered_map<uintptr_t, InputRange> leaf_input_ranges{};
};

struct Diagram {
  TreeNodeEx output_tree{};
};

auto Calculate(const CalculatorInput &input) -> std::vector<Diagram>;
}  // namespace vh::ponc::flow

#endif  // VH_PONC_FLOW_CALCULATOR_H_
