#include "core_i_family.h"

namespace esc::core {
///
auto IFamily::GetId() const -> FamilyId { return id_; }

///
IFamily::IFamily(FamilyId id) : id_{id} {}
}  // namespace esc::core