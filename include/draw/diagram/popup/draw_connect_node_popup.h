#ifndef VH_DRAW_CONNECT_NODE_POPUP_H_
#define VH_DRAW_CONNECT_NODE_POPUP_H_

#include "coreui_linker.h"
#include "draw_i_popup.h"

namespace esc::draw {
///
class ConnectNodePopup : public IPopup {
 public:
  ///
  void Draw(coreui::Linker &linker,
            const std::vector<coreui::FamilyGroup> &family_groups);

 private:
  ///
  bool for_all_pins_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_CONNECT_NODE_POPUP_H_
