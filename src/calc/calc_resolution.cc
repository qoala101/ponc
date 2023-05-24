/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "calc_resolution.h"

#include <algorithm>
#include <iterator>

namespace vh::ponc::calc {
namespace {
///
constexpr auto kResolution = 100;
}  // namespace

///
auto ToCalculatorResolution(float value) -> int {
  return static_cast<int>(value * kResolution);
}

///
auto ToCalculatorResolution(const std::vector<float> &values)
    -> std::vector<int> {
  auto calc_values = std::vector<int>{};
  calc_values.reserve(values.size());

  std::transform(values.cbegin(), values.cend(),
                 std::back_inserter(calc_values), [](const auto value) {
                   return vh::ponc::calc::ToCalculatorResolution(value);
                 });

  return calc_values;
}

///
auto FromCalculatorResolution(int value) -> float {
  return static_cast<float>(value) / kResolution;
}
}  // namespace vh::ponc::calc