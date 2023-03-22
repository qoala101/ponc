#ifndef VH_PONC_COREUI_PIN_H_
#define VH_PONC_COREUI_PIN_H_

#include <imgui.h>
#include <imgui_node_editor.h>

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "coreui_i_pin_traits.h"

namespace ne = ax::NodeEditor;

namespace vh::ponc::coreui {
///
struct PinFlowData {
  ///
  ne::PinId id{};
  ///
  ImColor color{};
  ///
  bool filled{};
};

///
struct Pin {
  ///
  std::optional<PinFlowData> flow_data{};
  ///
  std::variant<std::monostate, float, float *> value{};
  ///
  std::optional<PinLabel> label{};
};
}  // namespace vh::ponc::coreui

#endif  // VH_PONC_COREUI_PIN_H_
