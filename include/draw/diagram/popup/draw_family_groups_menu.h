#ifndef VH_DRAW_FAMILY_GROUPS_MENU_H_
#define VH_DRAW_FAMILY_GROUPS_MENU_H_

#include "coreui_family.h"
#include "cpp_callbacks.h"
#include "cpp_no_instances.h"

namespace esc::draw {
///
struct FamilyGroupsMenu : public cpp::NoInstances {
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
}  // namespace esc::draw

#endif  // VH_DRAW_FAMILY_GROUPS_MENU_H_