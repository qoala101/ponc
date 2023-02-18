#ifndef VH_COREUI_FLOW_OUTPUT_PIN_DRAWER_H_
#define VH_COREUI_FLOW_OUTPUT_PIN_DRAWER_H_

#include <imgui_node_editor.h>

#include "coreui_i_pin_drawer.h"
#include "flow_node_flow.h"

namespace esc::coreui {
// ---
class FlowOutputPinDrawer : public IPinDrawer {
 public:
  // ---
  explicit FlowOutputPinDrawer(const flow::NodeFlow& flow, ne::PinId pin_id);

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

#endif  // VH_COREUI_FLOW_OUTPUT_PIN_DRAWER_H_
