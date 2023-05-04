#ifndef VH_PONC_CALC_RESOLUTION_H_
#define VH_PONC_CALC_RESOLUTION_H_

#include <vector>

namespace vh::ponc::calc {
///
auto ToCalculatorResolution(float value) -> int;
///
auto FromCalculatorResolution(int value) -> float;
}  // namespace vh::ponc::calc

#endif  // VH_PONC_CALC_RESOLUTION_H_
