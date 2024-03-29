/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "core_i_family_group.h"

#include <memory>

#include "core_free_pin_family_group.h"

namespace vh::ponc::core {
///
auto IFamilyGroup::CreateDefaultFamilyGroups()
    -> std::vector<std::unique_ptr<IFamilyGroup>> {
  auto family_groups = std::vector<std::unique_ptr<IFamilyGroup>>{};
  family_groups.reserve(1);

  family_groups.emplace_back(std::make_unique<FreePinFamilyGroup>());
  return family_groups;
}

}  // namespace vh::ponc::core