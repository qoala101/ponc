/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#include "core_i_node.h"

#include <algorithm>
#include <ranges>
#include <vector>

namespace esc::core {
// ---
auto INode::GetId() const -> ne::NodeId { return id_; }

// ---
auto INode::GetInputPinId() const -> const std::optional<ne::PinId>& {
  return input_pin_id_;
}

// ---
auto INode::GetOutputPinIds() const -> const std::vector<ne::PinId>& {
  return output_pin_ids_;
}

// ---
auto INode::GetPosition() const -> ImVec2 { return ne::GetNodePosition(id_); }

// ---
void INode::SetPosition(const ImVec2& position) {
  ne::SetNodePosition(id_, position);
}

// ---
auto INode::GetInitialFlow() const -> flow::NodeFlow {
  auto initial_flow = flow::NodeFlow{};

  if (input_pin_id_.has_value()) {
    initial_flow.input_pin_flow.emplace(input_pin_id_->Get(), 0.0F);
  }

  for (const auto pin_id : output_pin_ids_) {
    initial_flow.output_pin_flows.emplace(pin_id.Get(), 0.0F);
  }

  return initial_flow;
}

// ---
INode::INode(ne::NodeId id, std::vector<ne::PinId> output_pin_ids)
    : id_{id}, output_pin_ids_{std::move(output_pin_ids)} {}

// ---
INode::INode(ne::NodeId id, ne::PinId input_pin_id,
             std::vector<ne::PinId> output_pin_ids)
    : id_{id},
      input_pin_id_{input_pin_id},
      output_pin_ids_{std::move(output_pin_ids)} {}

// ---
void INode::SetInitialFlowValues(flow::NodeFlow& /*unused*/) const {}

// ---
auto GetAllPinIds(const INode& node) -> std::vector<ne::PinId> {
  const auto& input_pin_id = node.GetInputPinId();
  auto pin_ids = std::vector<ne::PinId>{};

  if (input_pin_id.has_value()) {
    pin_ids.emplace_back(*input_pin_id);
  }

  for (const auto pin_id : node.GetOutputPinIds()) {
    pin_ids.emplace_back(pin_id);
  }

  return pin_ids;
}
}  // namespace esc::core