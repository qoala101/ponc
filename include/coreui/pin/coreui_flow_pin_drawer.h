#ifndef VH_COREUI_FLOW_PIN_DRAWER_H_
#define VH_COREUI_FLOW_PIN_DRAWER_H_

#include <imgui_node_editor.h>

#include "coreui_i_pin_drawer.h"

namespace esc::coreui {
class FlowPinDrawer : public IPinDrawer {
 public:
  explicit FlowPinDrawer(ne::PinId pin_id);

  auto GetPinId() const -> std::optional<ne::PinId> override;

 private:
  ne::PinId pin_id_{};
};
}  // namespace esc::coreui

#endif  // VH_COREUI_FLOW_PIN_DRAWER_H_
