#ifndef VH_DRAW_FLOW_OUTPUT_PIN_DRAWER_H_
#define VH_DRAW_FLOW_OUTPUT_PIN_DRAWER_H_

#include "draw_i_pin_drawer.h"

namespace esc::draw {
// ---
class FlowOutputPinDrawer : public IPinDrawer {
 public:
  // ---
  explicit FlowOutputPinDrawer(float value = {});

  // ---
  auto GetLabel [[nodiscard]] () const -> std::string override;
  // ---
  auto GetKind [[nodiscard]] () const -> ne::PinKind override;
  // ---
  auto GetFloat [[nodiscard]] () -> float* override;
  // ---
  auto IsEditable [[nodiscard]] () const -> bool override;

 private:
  float value_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_FLOW_OUTPUT_PIN_DRAWER_H_
