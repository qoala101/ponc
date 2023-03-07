#ifndef VH_COREUI_PIN_H_
#define VH_COREUI_PIN_H_

#include <imgui.h>
#include <imgui_node_editor.h>

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace esc::coreui {
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
  std::string label{};
};
}  // namespace esc::coreui

#endif  // VH_COREUI_PIN_H_
