#include "core_i_family_group.h"

#include <memory>

#include "core_free_pin_family_group.h"
#include "core_hub_family_group.h"

namespace vh::ponc::core {
///
auto IFamilyGroup::CreateDefaultFamilyGroups()
    -> std::vector<std::unique_ptr<IFamilyGroup>> {
  auto family_groups = std::vector<std::unique_ptr<IFamilyGroup>>{};
  family_groups.emplace_back(std::make_unique<FreePinFamilyGroup>());
  return family_groups;
}

}  // namespace vh::ponc::core