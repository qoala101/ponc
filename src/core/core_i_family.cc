/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#include "core_i_family.h"

namespace esc::core {
auto IFamily::GetId() const -> FamilyId { return id_; }

IFamily::IFamily(FamilyId id) : id_{id} {}

// // auto IsChildOf(ne::NodeId node_id, const IFamily &family) -> bool {
//   return family.FindNode(node_id) != nullptr;
// }
}  // namespace esc::core