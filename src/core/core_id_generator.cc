#include "core_id_generator.h"

namespace esc::core {
///
IdGenerator::IdGenerator(uintptr_t next_id) : next_id_{next_id} {}
}  // namespace esc::core