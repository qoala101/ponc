/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_STYLE_DEFAULT_COLORS_H_
#define VH_PONC_STYLE_DEFAULT_COLORS_H_

#include <imgui.h>

#include "style_tailwind.h"

namespace vh::ponc::style {
///
struct DefaultColors : cpp::StaticApi {
  ///
  static constexpr auto kGray = Tailwind::Color::kZinc;
  ///
  static constexpr auto kAccent = Tailwind::Color::kSky;
  ///
  static constexpr auto kWhite =
      Tailwind::GetColor(kGray, Tailwind::Shade::k50);
  ///
  static constexpr auto kError =
      Tailwind::GetColor(Tailwind::Color::kRed, Tailwind::Shade::k500);
  ///
  static constexpr auto kSuccess =
      Tailwind::GetColor(Tailwind::Color::kGreen, Tailwind::Shade::k500);
  ///
  static constexpr auto kTransparent = ImU32{};
};
}  // namespace vh::ponc::style

#endif  // VH_PONC_STYLE_DEFAULT_COLORS_H_
