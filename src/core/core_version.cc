#include "core_version.h"

#include <type_traits>

namespace vh::ponc::core {
///
auto GetCurrentVersion() -> Version {
  using T = std::underlying_type_t<Version>;

  return static_cast<Version>(static_cast<T>(Version::kAfterCurrent) - 1);
}
}  // namespace vh::ponc::core