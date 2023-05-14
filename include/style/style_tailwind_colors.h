#ifndef VH_PONC_STYLE_TAILWIND_COLORS_H_
#define VH_PONC_STYLE_TAILWIND_COLORS_H_

#include <imgui.h>

#include "cpp_no_instances.h"

namespace vh::ponc::style {
///
struct TailwindColors : cpp::NoInstances {
  // NOLINTBEGIN(*-signed-bitwise)
  ///
  static constexpr auto kRed500 = IM_COL32(239, 68, 68, 255);
  ///
  static constexpr auto kAmber500 = IM_COL32(245, 158, 11, 255);
  ///
  static constexpr auto kGreen500 = IM_COL32(34, 197, 94, 255);
  ///
  static constexpr auto kEmerald700 = IM_COL32(4, 120, 87, 255);
  ///
  static constexpr auto kCyan500 = IM_COL32(6, 182, 212, 255);
  ///
  static constexpr auto kLightBlue500 = IM_COL32(14, 165, 233, 255);
  ///
  static constexpr auto kBlue800 = IM_COL32(30, 64, 175, 255);
  ///
  static constexpr auto kBlue600 = IM_COL32(37, 99, 235, 255);
  ///
  static constexpr auto kBlue500 = IM_COL32(59, 130, 246, 255);
  ///
  static constexpr auto kIndigo700 = IM_COL32(67, 56, 202, 255);
  ///
  static constexpr auto kPurple800 = IM_COL32(107, 33, 168, 255);
  ///
  static constexpr auto kPurple600 = IM_COL32(147, 51, 234, 255);
  ///
  static constexpr auto kRose700 = IM_COL32(190, 18, 60, 255);
  ///
  static constexpr auto kGray900 = IM_COL32(24, 24, 27, 255);
  ///
  static constexpr auto kGray700 = IM_COL32(63, 63, 70, 255);
  ///
  static constexpr auto kGray600 = IM_COL32(82, 82, 91, 255);
  ///
  static constexpr auto kGray500 = IM_COL32(113, 113, 122, 255);
  ///
  static constexpr auto kGray50 = IM_COL32(250, 250, 250, 255);
  // NOLINTEND(*-signed-bitwise)
};
}  // namespace vh::ponc::style

#endif  // VH_PONC_STYLE_TAILWIND_COLORS_H_
