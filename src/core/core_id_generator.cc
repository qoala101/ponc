#include "core_id_generator.h"

#include "core_id_value.h"

namespace vh::ponc::core {
///
IdGenerator::IdGenerator(UnspecifiedIdValue next_id) : next_id_{next_id} {}
}  // namespace vh::ponc::core