#ifndef VH_PONC_STYLE_DEFAULT_COLORS_H_
#define VH_PONC_STYLE_DEFAULT_COLORS_H_

#include "style_tailwind_colors.h"

namespace vh::ponc::style {
///
struct DefaultColors : cpp::NoInstances {
  ///
  static constexpr auto kWhite = TailwindColors::kGray50;
  ///
  static constexpr auto kBlack = TailwindColors::kGray900;
  ///
  static constexpr auto kError = TailwindColors::kRed500;
  ///
  static constexpr auto kSuccess = TailwindColors::kGreen500;
};
}  // namespace vh::ponc::style

#endif  // VH_PONC_STYLE_DEFAULT_COLORS_H_
