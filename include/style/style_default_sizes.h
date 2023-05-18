/**
 * PON Calculator https://github.com/qoala101/ponc
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_STYLE_DEFAULT_SIZES_H_
#define VH_PONC_STYLE_DEFAULT_SIZES_H_

#include "cpp_static_api.h"

namespace vh::ponc::style {
///
struct DefaultSizes : cpp::StaticApi {
  ///
  static constexpr auto kNormalThickness = 2;
  ///
  static constexpr auto kBoldThickness = 4;
};
}  // namespace vh::ponc::style

#endif  // VH_PONC_STYLE_DEFAULT_SIZES_H_
