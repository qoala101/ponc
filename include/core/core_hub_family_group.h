#ifndef VH_PONC_CORE_HUB_FAMILY_GROUP_H_
#define VH_PONC_CORE_HUB_FAMILY_GROUP_H_

#include <memory>

#include "core_i_family.h"
#include "core_i_family_group.h"
#include "json_i_family_parser.h"

namespace vh::ponc::core {
///
class HubFamilyGroup : public core::IFamilyGroup {
 public:
  ///
  auto CreateFamilies(core::IdGenerator &id_generator) const
      -> std::vector<std::unique_ptr<core::IFamily>> override;
  ///
  auto CreateFamilyParser() const
      -> std::unique_ptr<json::IFamilyParser> override;
};
}  // namespace vh::ponc::core

#endif  // VH_PONC_CORE_HUB_FAMILY_GROUP_H_