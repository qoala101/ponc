/**
 * PON Calculator https://github.com/qoala101/ponc
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_FLOW_NODE_FLOW_H_
#define VH_PONC_FLOW_NODE_FLOW_H_

#include <imgui_node_editor.h>

#include <cstdint>
#include <map>
#include <optional>
#include <unordered_map>

#include "core_id_value.h"

namespace ne = ax::NodeEditor;

namespace vh::ponc::flow {
///
using PinFlow = std::pair<core::IdValue<ne::PinId>, float>;
///
using PinFlows = std::unordered_map<PinFlow::first_type, PinFlow::second_type>;

///
struct NodeFlow {
  ///
  static auto GetPinFlow(const NodeFlow &flow, ne::PinId pin_id) -> float;

  ///
  std::optional<PinFlow> input_pin_flow{};
  ///
  PinFlows output_pin_flows{};

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

///
using NodeFlows = std::unordered_map<core::IdValue<ne::NodeId>, flow::NodeFlow>;
}  // namespace vh::ponc::flow

#endif  // VH_PONC_FLOW_NODE_FLOW_H_
