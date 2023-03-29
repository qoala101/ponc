#ifndef VH_PONC_FLOW_CALCULATOR_H_
#define VH_PONC_FLOW_CALCULATOR_H_

#include <cstdint>
#include <map>
#include <unordered_map>

#include "core_i_family.h"

namespace vh::ponc::flow {
struct TreeNodeEx {
  core::FamilyId family_id{};
  std::vector<float> outputs{};
  float cost{};
  std::map<int, TreeNodeEx> child_nodes{};

  auto EmplaceChild(int index, TreeNodeEx child) -> TreeNodeEx &;
  auto AreOutputsLessThan(float value) const -> bool;
};

struct InputRange {
  float min{};
  float max{};
};

struct FamilyFlow {
  core::FamilyId family_id{};
  std::vector<float> outputs{};
  float cost{};
};

struct CalculatorInput {
  std::vector<FamilyFlow> family_flows{};
  std::vector<InputRange> input_ranges{};
};

auto Calculate(const CalculatorInput &input) -> std::vector<TreeNodeEx>;
}  // namespace vh::ponc::flow

#endif  // VH_PONC_FLOW_CALCULATOR_H_
