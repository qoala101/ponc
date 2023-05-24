/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_DRAW_FAMILY_GROUPS_MENU_H_
#define VH_PONC_DRAW_FAMILY_GROUPS_MENU_H_

#include <optional>
#include <unordered_set>
#include <vector>

#include "core_i_family.h"
#include "coreui_family.h"
#include "cpp_callbacks.h"
#include "cpp_static_api.h"

namespace vh::ponc::draw {
///
struct FamilyGroupsMenu : public cpp::StaticApi {
  ///
  struct Callbacks {
    ///
    std::optional<cpp::Query<bool, const core::IFamily&>> is_family_enabled{};
    ///
    cpp::Signal<const coreui::Family&> family_selected{};
  };

  ///
  static void Draw(const std::vector<coreui::FamilyGroup>& family_groups,
                   const Callbacks& callbacks);

 private:
  ///
  static auto GetDisabledFamilies(const std::vector<coreui::Family>& families,
                                  const Callbacks& callbacks);
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_FAMILY_GROUPS_MENU_H_
