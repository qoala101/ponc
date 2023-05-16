#ifndef VH_PONC_STYLE_DEFAULT_COLORS_H_
#define VH_PONC_STYLE_DEFAULT_COLORS_H_

#include "style_tailwind.h"

namespace vh::ponc::style {
///
struct DefaultColors : cpp::StaticApi {
  ///
  static constexpr auto kWhite =
      Tailwind::GetColor(Tailwind::Color::kZinc, Tailwind::Shade::k50);
  ///
  static constexpr auto kBlack =
      Tailwind::GetColor(Tailwind::Color::kZinc, Tailwind::Shade::k900);
  ///
  static constexpr auto kError =
      Tailwind::GetColor(Tailwind::Color::kRed, Tailwind::Shade::k500);
  ///
  static constexpr auto kSuccess =
      Tailwind::GetColor(Tailwind::Color::kGreen, Tailwind::Shade::k500);
};
}  // namespace vh::ponc::style

#endif  // VH_PONC_STYLE_DEFAULT_COLORS_H_
