#ifndef VH_PONC_STYLE_TAILWIND_H_
#define VH_PONC_STYLE_TAILWIND_H_

#include <imgui.h>

#include "cpp_assert.h"
#include "cpp_static_api.h"
#include "style_tailwind_colors.h"

namespace vh::ponc::style {
///
struct Tailwind : cpp::StaticApi {
  ///
  enum class Color {
    kSlate = 0,
    kGray,
    kZinc,
    kNeutral,
    kStone,
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
    k50 = 0,
    k100,
    k200,
    k300,
    k400,
    k500,
    k600,
    k700,
    k800,
    k900,
    k950,
    kNumShades
  };

  ///
  static constexpr auto GetColor(Color color, Shade shade) {
    static_assert(kTailwindColors.size() >=
                  static_cast<int>(Color::kNumColors));

    // NOLINTNEXTLINE(*-constant-array-index)
    const auto &shades = kTailwindColors[static_cast<int>(color)];

    Expects(shades.size() >= static_cast<int>(Shade::kNumShades));
    return shades[static_cast<int>(shade)];
  }
};
}  // namespace vh::ponc::style

#endif  // VH_PONC_STYLE_TAILWIND_H_
