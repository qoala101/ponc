#include "core_id_generator.h"

namespace vh::ponc::core {
///
IdGenerator::IdGenerator(uintptr_t next_id) : next_id_{next_id} {}
}  // namespace vh::ponc::core