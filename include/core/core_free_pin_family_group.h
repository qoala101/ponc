#ifndef VH_CORE_FREE_PIN_FAMILY_GROUP_H_
#define VH_CORE_FREE_PIN_FAMILY_GROUP_H_

#include <memory>

#include "core_i_family.h"
#include "core_i_family_group.h"
#include "json_i_family_parser.h"

namespace esc::core {
///
class FreePinFamilyGroup : public core::IFamilyGroup {
 public:
  ///
  auto CreateFamilies(core::IdGenerator &id_generator) const
      -> std::vector<std::unique_ptr<core::IFamily>> override;
  ///
  auto CreateFamilyParser() const
      -> std::unique_ptr<json::IFamilyParser> override;
};
}  // namespace esc::core

#endif  // VH_CORE_FREE_PIN_FAMILY_GROUP_H_
