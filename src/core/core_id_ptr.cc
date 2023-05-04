#include "core_id_ptr.h"

#include <type_traits>

#include "core_i_family.h"

namespace vh::ponc::core {
///
auto GetValue(const IdPtr &id) -> UnspecifiedIdValue {
  return std::visit([](const auto *id) { return id->Get(); }, id);
}

///
void SetValue(const IdPtr &id, UnspecifiedIdValue value) {
  std::visit([value](auto *id) { (*id) = value; }, id);
}
}  // namespace vh::ponc::core