/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_JSON_VERSIFIER_H_
#define VH_PONC_JSON_VERSIFIER_H_

#include <crude_json.h>

#include "cpp_static_api.h"

namespace vh::ponc::json {
///
enum class Version { kPreCalculator = 0, kCalculatorAdded, kAfterCurrent };

///
struct Versifier : public cpp::StaticApi {
  ///
  static auto GetCurrentVersion() -> Version;
  ///
  static void UpgradeToCurrentVersion(crude_json::value& project_json);
};
}  // namespace vh::ponc::json

#endif  // VH_PONC_JSON_VERSIFIER_H_
