#ifndef VH_PONC_STYLE_TAILWIND_COLORS_H_
#define VH_PONC_STYLE_TAILWIND_COLORS_H_

#include <imgui.h>

#include "cpp_no_instances.h"

namespace vh::ponc::style {
///
struct TailwindColors : cpp::NoInstances {
  ///
  enum class Color {
    kZinc = 0,
    kRed,
    kOrange,
    kAmber,
    kYellow,
    kLime,
    kGreen,
    kEmerald,
    kTeal,
    kCyan,
    kSky,
    kBlue,
    kIndigo,
    kViolet,
    kPurple,
    kFuchsia,
    kPink,
    kRose,
    kNumColors
  };

  ///
  enum class Shade {
    k950 = 0,
    k900,
    k800,
    k700,
    k600,
    k500,
    k400,
    k300,
    k200,
    k100,
    k50,
    kNumShades
  };

  ///
  template <Color color, Shade shade>
  static constexpr auto GetColor() -> ImU32;
};
}  // namespace vh::ponc::style

#endif  // VH_PONC_STYLE_TAILWIND_COLORS_H_
