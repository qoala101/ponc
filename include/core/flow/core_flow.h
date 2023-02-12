#ifndef VH_CORE_FLOW_H_
#define VH_CORE_FLOW_H_

#include <imgui_node_editor.h>

#include <optional>
#include <vector>

namespace ne = ax::NodeEditor;

namespace esc::core {
// ---
struct PinFlow {
  ne::PinId id{};
  float value{};
};

// ---
struct NodePinFlows {
  std::optional<PinFlow> input_pin_flow{};
  std::vector<PinFlow> output_pin_flows{};
};
}  // namespace esc::core

#endif  // VH_CORE_FLOW_H_
