#include "coreui_i_family_traits.h"

#include <algorithm>
#include <ranges>

namespace esc::coreui {
auto IFamilyTraits::GetGroupLabel() const -> std::string { return GetLabel(); }

auto IFamilyTraits::IsUserAccessible() -> bool { return true; }

auto GroupByLabels(const std::vector<std::shared_ptr<core::IFamily>>& families)
    -> std::vector<
        std::pair<std::string, std::vector<std::shared_ptr<core::IFamily>>>> {
  auto family_groups = std::vector<
      std::pair<std::string, std::vector<std::shared_ptr<core::IFamily>>>>{};

  for (const auto& family : families) {
    const auto drawer = family->CreateUiTraits();

    if (!drawer->IsUserAccessible()) {
      continue;
    }

    const auto group_label = drawer->GetGroupLabel();
    const auto group =
        std::ranges::find_if(family_groups, [&group_label](const auto& group) {
          return group.first == group_label;
        });

    if (group != family_groups.end()) {
      group->second.emplace_back(family);
      continue;
    }

    family_groups.emplace_back(
        group_label, std::vector<std::shared_ptr<core::IFamily>>{family});
  }

  return family_groups;
}
}  // namespace esc::coreui