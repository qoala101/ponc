#ifndef VH_PONC_DRAW_CONNECT_NODE_POPUP_H_
#define VH_PONC_DRAW_CONNECT_NODE_POPUP_H_

#include "coreui_linker.h"
#include "draw_i_popup.h"

namespace vh::ponc::draw {
///
class ConnectNodePopup : public IPopup {
 public:
  ///
  static auto GetLabel() -> std::string;

  ///
  void Draw(coreui::Linker &linker,
            const std::vector<coreui::FamilyGroup> &family_groups);

 private:
  ///
  bool for_all_pins_{};
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_CONNECT_NODE_POPUP_H_
