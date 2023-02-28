#include "coreui_i_family_traits.h"

#include <algorithm>
#include <ranges>

namespace esc::coreui {
///
auto IFamilyTraits::GroupByLabels(
    const std::vector<std::unique_ptr<core::IFamily>>& families)
    -> std::vector<std::pair<std::string, std::vector<core::IFamily*>>> {
  auto family_groups =
      std::vector<std::pair<std::string, std::vector<core::IFamily*>>>{};

  for (const auto& family : families) {
    const auto group_label = family->CreateUiTraits()->GetGroupLabel();
    const auto group = std::find_if(family_groups.begin(), family_groups.end(),
                                    [&group_label](const auto& group) {
                                      return group.first == group_label;
                                    });

    if (group == family_groups.end()) {
      family_groups.emplace_back(group_label, std::vector{family.get()});
      continue;
    }

    group->second.emplace_back(family);
  }

  return family_groups;
}

///
auto IFamilyTraits::GetGroupLabel() const -> std::string { return GetLabel(); }

///
auto IFamilyTraits::IsUserAccessible() -> bool { return true; }
}  // namespace esc::coreui