#ifndef VH_DRAW_FLOW_OUTPUT_PIN_DRAWER_H_
#define VH_DRAW_FLOW_OUTPUT_PIN_DRAWER_H_

#include <imgui_node_editor.h>

#include "core_flow.h"
#include "coreui_i_pin_drawer.h"

namespace esc::coreui {
// ---
class FlowOutputPinDrawer : public IPinDrawer {
 public:
  // ---
  explicit FlowOutputPinDrawer(const core::Flow& flow, ne::PinId pin_id);

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
