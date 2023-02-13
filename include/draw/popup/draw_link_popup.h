#ifndef VH_DRAW_LINK_POPUP_H_
#define VH_DRAW_LINK_POPUP_H_

#include "core_link.h"
#include "draw_i_popup.h"

namespace esc::draw {
// ---
class LinkPopup : public IPopup {
 public:
  explicit LinkPopup(core::Link link);

 private:
  // ---
  auto GetLabel [[nodiscard]] (State &state) const -> std::string override;
  // ---
  void DrawContent(State &state) override;

  core::Link link_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_LINK_POPUP_H_
