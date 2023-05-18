/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_STYLE_FLOW_COLORS_H_
#define VH_PONC_STYLE_FLOW_COLORS_H_

#include <imgui.h>

#include "cpp_static_api.h"
#include "style_utils.h"

namespace vh::ponc::style {
///
struct FlowColors : cpp::StaticApi {
  ///
  static constexpr auto kMin = FromRgb(0, 0, 255);
  ///
  static constexpr auto kLow = FromRgb(0, 255, 255);
  ///
  static constexpr auto kGood = FromRgb(0, 255, 0);
  ///
  static constexpr auto kHigh = FromRgb(255, 255, 0);
  ///
  static constexpr auto kMax = FromRgb(255, 0, 0);
};
}  // namespace vh::ponc::style

#endif  // VH_PONC_STYLE_FLOW_COLORS_H_
