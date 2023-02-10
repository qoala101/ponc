#include "core_node.h"

INode::INode(ne::NodeId id, std::vector<std::shared_ptr<Pin>> input_pins,
           std::vector<std::shared_ptr<Pin>> output_pins)
    : id_{id},
      input_pins_{std::move(input_pins)},
      output_pins_{std::move(output_pins)} {}

auto INode::GetId() const -> ne::NodeId { return id_; }

auto INode::GetInputPins() const -> const std::vector<std::shared_ptr<Pin>>& {
  return input_pins_;
}

auto INode::GetInputPins() -> std::vector<std::shared_ptr<Pin>>& {
  return input_pins_;
}

auto INode::GetOutputPins() const -> const std::vector<std::shared_ptr<Pin>>& {
  return output_pins_;
}

auto INode::GetOutputPins() -> std::vector<std::shared_ptr<Pin>>& {
  return output_pins_;
}