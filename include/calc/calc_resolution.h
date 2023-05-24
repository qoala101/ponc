/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_CALC_RESOLUTION_H_
#define VH_PONC_CALC_RESOLUTION_H_

#include <vector>

namespace vh::ponc::calc {
///
auto ToCalculatorResolution(float value) -> int;
///
auto ToCalculatorResolution(const std::vector<float> &values)
    -> std::vector<int>;
///
auto FromCalculatorResolution(int value) -> float;
}  // namespace vh::ponc::calc

#endif  // VH_PONC_CALC_RESOLUTION_H_
