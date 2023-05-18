/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "core_id_generator.h"

#include "core_id_value.h"

namespace vh::ponc::core {
///
IdGenerator::IdGenerator(UnspecifiedIdValue next_id) : next_id_{next_id} {}
}  // namespace vh::ponc::core