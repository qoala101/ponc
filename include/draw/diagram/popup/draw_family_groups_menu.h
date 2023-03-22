#ifndef VH_PONC_DRAW_FAMILY_GROUPS_MENU_H_
#define VH_PONC_DRAW_FAMILY_GROUPS_MENU_H_

#include "coreui_family.h"
#include "cpp_callbacks.h"
#include "cpp_no_instances.h"

namespace vh::ponc::draw {
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
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_FAMILY_GROUPS_MENU_H_
