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
  core::FamilyId family_id{};
  std::vector<float> outputs{};
  float cost{};

  auto GetMinDecrementOutput() const -> float;
  auto GetMaxDecrementOutput() const -> float;
};

struct TreeNodeEx {
  core::FamilyId family_id{};
  std::vector<float> outputs{};
  float cost{};
  std::map<int, TreeNodeEx> child_nodes{};

  explicit TreeNodeEx(const FamilyFlow &family_flow);
  TreeNodeEx() = default;
  auto EmplaceChild(int index, TreeNodeEx child) -> TreeNodeEx &;
  auto AreOutputsLessThan(float value) const -> bool;
  auto GetNumChildren() const -> int;
  auto GetNumOutputs() const -> int;
  auto GetChildIndex(const TreeNodeEx *child) const -> int;
  auto CalculateCost() const -> float;

 private:
  ///
  friend auto operator==(const TreeNodeEx &, const TreeNodeEx &)
      -> bool = default;
};

struct CalculatorInput {
  TreeNodeEx root{};
  TreeNodeEx client{};
  std::vector<FamilyFlow> family_flows{};
  std::vector<InputRange> input_ranges{};
};

auto Calculate(const CalculatorInput &input) -> std::vector<TreeNodeEx>;
}  // namespace vh::ponc::flow

#endif  // VH_PONC_FLOW_CALCULATOR_H_
