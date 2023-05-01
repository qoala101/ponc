#ifndef VH_PONC_CORE_VERSION_H_
#define VH_PONC_CORE_VERSION_H_

namespace vh::ponc::core {
///
enum class Version { kPreCalculator = 0, kCalculatorAdded, kAfterCurrent };

///
auto GetCurrentVersion() -> Version;
}  // namespace vh::ponc::core

#endif  // VH_PONC_CORE_VERSION_H_
