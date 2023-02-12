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
struct NodeFlow {
  std::optional<PinFlow> parent_flow{};
  std::vector<PinFlow> child_flows{};
};
}  // namespace esc::core

#endif  // VH_CORE_FLOW_H_
