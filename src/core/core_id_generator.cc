/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#include "core_id_generator.h"

namespace esc::core {
IdGenerator::IdGenerator(uintptr_t next_value) : next_value_{next_value} {}
}  // namespace esc::core