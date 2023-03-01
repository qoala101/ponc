#ifndef VH_CORE_FLOW_H_
#define VH_CORE_FLOW_H_

#include <imgui_node_editor.h>

#include <cstdint>
#include <map>
#include <optional>
#include <unordered_map>

namespace ne = ax::NodeEditor;

namespace esc::flow {
///
struct NodeFlow {
  ///
  static auto GetPinFlow(const NodeFlow &flow, ne::PinId pin_id) -> float;

  ///
  std::optional<std::pair<uintptr_t, float>> input_pin_flow{};
  ///
  std::unordered_map<uintptr_t, float> output_pin_flows{};

 private:
  ///
  friend auto operator+(NodeFlow &left, const NodeFlow &right) -> NodeFlow &;
  ///
  friend auto operator+(NodeFlow &left, float right) -> NodeFlow &;
  ///
  friend void operator+=(NodeFlow &left, const NodeFlow &right);
  ///
  friend void operator+=(NodeFlow &left, float right);
};
}  // namespace esc::flow

#endif  // VH_CORE_FLOW_H_
