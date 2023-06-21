/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "core_i_node.h"

#include <imgui.h>
#include <imgui_node_editor.h>

#include <algorithm>
#include <iterator>
#include <optional>
#include <unordered_map>
#include <variant>
#include <vector>

#include "cpp_assert.h"
#include "json_i_node_writer.h"

namespace vh::ponc::core {
///
auto INode::GetIds(INode& node) -> std::vector<IdPtr> {
  const auto node_ids = node.GetIds();

  auto ids = std::vector<IdPtr>{node_ids.node_id};
  ids.reserve(node_ids.output_pin_ids.size() + 2);

  if (node_ids.input_pin_id.has_value()) {
    ids.emplace_back(*node_ids.input_pin_id);
  }

  std::transform(node_ids.output_pin_ids.cbegin(),
                 node_ids.output_pin_ids.cend(), std::back_inserter(ids),
                 [](auto* pin_id) { return pin_id; });

  return ids;
}

///
auto INode::GetAllPins(const INode& node)
    -> std::vector<std::pair<ne::PinId, ne::PinKind>> {
  auto pins = std::vector<std::pair<ne::PinId, ne::PinKind>>{};
  pins.reserve(node.output_pin_ids_.size() + 1);

  if (node.input_pin_id_.has_value()) {
    pins.emplace_back(*node.input_pin_id_, ne::PinKind::Input);
  }

  std::transform(node.output_pin_ids_.cbegin(), node.output_pin_ids_.cend(),
                 std::back_inserter(pins), [](const auto pin_id) {
                   return std::pair{pin_id, ne::PinKind::Output};
                 });

  return pins;
}

///
auto INode::GetPinKind(const INode& node, ne::PinId pin_id) -> ne::PinKind {
  if (node.input_pin_id_.has_value() && (*node.input_pin_id_ == pin_id)) {
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
    if (node.input_pin_id_.has_value()) {
      return *node.input_pin_id_;
    }

    return std::nullopt;
  }

  const auto& output_pins = node.output_pin_ids_;

  if (output_pins.empty()) {
    return std::nullopt;
  }

  return output_pins.front();
}

auto INode::CreateWriter() const -> std::unique_ptr<json::INodeWriter> {
  return std::make_unique<json::INodeWriter>();
}

///
auto INode::GetId() const -> ne::NodeId { return id_; }

auto INode::GetIds() -> NodeIds {
  auto ids = NodeIds{.node_id = &id_};

  if (input_pin_id_.has_value()) {
    ids.input_pin_id = &*input_pin_id_;
  }

  std::transform(output_pin_ids_.begin(), output_pin_ids_.end(),
                 std::back_inserter(ids.output_pin_ids),
                 [](auto& pin_id) { return &pin_id; });

  return ids;
}

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

  std::transform(output_pin_ids_.cbegin(), output_pin_ids_.cend(),
                 std::inserter(initial_flow.output_pin_flows,
                               initial_flow.output_pin_flows.begin()),
                 [](const auto pin_id) {
                   return std::pair{pin_id.Get(), 0};
                 });

  SetInitialFlowValues(initial_flow);
  return initial_flow;
}

///
INode::INode(const ConstructorArgs& args)
    : id_{args.id},
      family_id_{args.family_id},
      input_pin_id_{args.input_pin_id},
      output_pin_ids_{args.output_pin_ids},
      pos_{args.pos} {}

///
void INode::SetInitialFlowValues(flow::NodeFlow& /*unused*/) const {}
}  // namespace vh::ponc::core