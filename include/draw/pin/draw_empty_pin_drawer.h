#ifndef VH_DRAW_EMPTY_PIN_DRAWER_H_
#define VH_DRAW_EMPTY_PIN_DRAWER_H_

#include "draw_i_pin_drawer.h"

namespace esc::draw {
// ---
class EmptyPinDrawer : public IPinDrawer {
 public:
  // ---
  auto GetLabel [[nodiscard]] () const -> std::string override;
  // ---
  auto GetFloat [[nodiscard]] () const -> float* override;
  // ---
  auto IsEditable [[nodiscard]] () const -> bool override;
};
}  // namespace esc::draw

#endif  // VH_DRAW_EMPTY_PIN_DRAWER_H_
