#ifndef VH_PONC_JSON_VERSIFIER_H_
#define VH_PONC_JSON_VERSIFIER_H_

#include "cpp_no_instances.h"
#include "crude_json.h"

namespace vh::ponc::json {
///
enum class Version { kPreCalculator = 0, kCalculatorAdded, kAfterCurrent };

///
struct Versifier : public cpp::NoInstances {
  ///
  static auto GetCurrentVersion() -> Version;
  ///
  static void UpgradeToCurrentVersion(crude_json::value& project_json);
};
}  // namespace vh::ponc::json

#endif  // VH_PONC_JSON_VERSIFIER_H_
