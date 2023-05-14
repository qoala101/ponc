#ifndef VH_PONC_STYLE_FLOW_COLORS_H_
#define VH_PONC_STYLE_FLOW_COLORS_H_

#include "cpp_no_instances.h"
#include "imgui.h"

namespace vh::ponc::style {
///
struct FlowColors : cpp::NoInstances {
  ///
  static constexpr auto kMin = IM_COL32(0, 0, 255, 255);
  ///
  static constexpr auto kLow = IM_COL32(0, 255, 255, 255);
  ///
  static constexpr auto kGood = IM_COL32(0, 255, 0, 255);
  ///
  static constexpr auto kHigh = IM_COL32(255, 255, 0, 255);
  ///
  static constexpr auto kMax = IM_COL32(255, 0, 0, 255);
};
}  // namespace vh::ponc::style

#endif  // VH_PONC_STYLE_FLOW_COLORS_H_
