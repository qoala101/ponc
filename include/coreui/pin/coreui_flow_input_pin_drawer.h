#ifndef VH_DRAW_FLOW_INPUT_PIN_DRAWER_H_
#define VH_DRAW_FLOW_INPUT_PIN_DRAWER_H_

#include "coreui_i_pin_drawer.h"
#include "flow_node_flow.h"
#include "imgui_node_editor.h"

namespace esc::coreui {
// ---
class FlowInputPinDrawer : public IPinDrawer {
 public:
  // ---
  explicit FlowInputPinDrawer(const flow::NodeFlow& flow);

  // ---
  auto GetLabel [[nodiscard]] () const -> std::string override;
  // ---
  auto GetKind [[nodiscard]] () const -> ne::PinKind override;
  // ---
  auto GetFloat [[nodiscard]] () -> float* override;
  // ---
  auto IsEditable [[nodiscard]] () const -> bool override;

  auto GetPinId [[nodiscard]] () const -> std::optional<ne::PinId> override {
    return pin_id_;
  }

 private:
  ne::PinId pin_id_{};
  float value_{};
};
}  // namespace esc::coreui

#endif  // VH_DRAW_FLOW_INPUT_PIN_DRAWER_H_
