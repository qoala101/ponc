#include "core_i_node.h"

#include <imgui_node_editor.h>

#include <algorithm>
#include <optional>
#include <ranges>
#include <vector>

#include "cpp_assert.h"

namespace esc::core {
///
auto INode::GetAllPins(const INode& node)
    -> std::vector<std::pair<ne::PinId, ne::PinKind>> {
  auto pins = std::vector<std::pair<ne::PinId, ne::PinKind>>{};

  const auto& output_pins = node.GetOutputPinIds();
  pins.reserve(output_pins.size() + 1);

  if (const auto& input_pin_id = node.GetInputPinId()) {
    pins.emplace_back(*input_pin_id, ne::PinKind::Input);
  }

  std::transform(output_pins.begin(), output_pins.end(),
                 std::back_inserter(pins), [](auto pin_id) {
                   return std::pair{pin_id, ne::PinKind::Output};
                 });

  return pins;
}

///
auto INode::GetPinKind(const INode& node, ne::PinId pin_id) -> ne::PinKind {
  const auto& input_pin_id = node.GetInputPinId();

  if (input_pin_id.has_value() && (*input_pin_id == pin_id)) {
    return ne::PinKind::Input;
  }

  return ne::PinKind::Output;
}

///
auto INode::GetFirstPinOfKind(const INode& node, ne::PinKind pin_kind)
    -> ne::PinId {
  const auto pin = FindFirstPinOfKind(node, pin_kind);
  Expects(pin.has_value());
  return *pin;
}

///
auto INode::FindFirstPinOfKind(const INode& node, ne::PinKind pin_kind)
    -> std::optional<ne::PinId> {
  if (pin_kind == ne::PinKind::Input) {
    if (const auto& input_pin = node.GetInputPinId()) {
      return *input_pin;
    }

    return std::nullopt;
  }

  const auto& output_pins = node.GetOutputPinIds();

  if (output_pins.empty()) {
    return std::nullopt;
  }

  return output_pins[0];
}

///
auto INode::GetId() const -> ne::NodeId { return id_; }

///
auto INode::GetFamilyId() const -> FamilyId { return family_id_; }

///
auto INode::GetInputPinId() const -> const std::optional<ne::PinId>& {
  return input_pin_id_;
}

///
auto INode::GetOutputPinIds() const -> const std::vector<ne::PinId>& {
  return output_pin_ids_;
}

///
auto INode::GetPos() const -> const ImVec2& { return pos_; }

///
void INode::SetPos(const ImVec2& pos) { pos_ = pos; }

///
auto INode::GetInitialFlow() const -> flow::NodeFlow {
  auto initial_flow = flow::NodeFlow{};

  if (input_pin_id_.has_value()) {
    initial_flow.input_pin_flow.emplace(input_pin_id_->Get(), 0);
  }

  for (const auto pin_id : output_pin_ids_) {
    initial_flow.output_pin_flows.emplace(pin_id, 0);
  }

  SetInitialFlowValues(initial_flow);
  return initial_flow;
}

///
INode::INode(ConstructorArgs args)
    : id_{args.id},
      family_id_{args.family_id},
      input_pin_id_{args.input_pin_id},
      output_pin_ids_{std::move(args.output_pin_ids)},
      pos_{args.pos} {}

///
void INode::SetInitialFlowValues(flow::NodeFlow& /*unused*/) const {}
}  // namespace esc::core