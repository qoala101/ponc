#include "calc_resolution.h"

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
auto FromCalculatorResolution(int value) -> float {
  return static_cast<float>(value) / kResolution;
}
}  // namespace vh::ponc::calc