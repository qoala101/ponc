#ifndef VH_DRAW_EMPTY_PIN_DRAWER_H_
#define VH_DRAW_EMPTY_PIN_DRAWER_H_

#include "draw_i_pin_drawer.h"
#include "imgui_node_editor.h"

namespace esc::draw {
// ---
class EmptyPinDrawer : public IPinDrawer {
 public:
  // ---
  explicit EmptyPinDrawer(ne::PinKind pin_kind);

  // ---
  auto GetKind [[nodiscard]] () const -> ne::PinKind override;

 private:
  ne::PinKind pin_kind_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_EMPTY_PIN_DRAWER_H_
