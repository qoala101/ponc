#include "core_i_family.h"

#include <optional>

namespace esc::core {
///
auto IFamily::GetType() const -> std::optional<FamilyType> {
  return std::nullopt;
}

///
auto IFamily::GetId() const -> FamilyId { return id_; }

///
IFamily::IFamily(FamilyId id) : id_{id} {}
}  // namespace esc::core