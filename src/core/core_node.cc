#include "core_node.h"

Node::Node(ne::NodeId id, std::vector<Pin> input_pins,
           std::vector<Pin> output_pins, std::string name, ImColor color)
    : id_{id},
      input_pins_{std::move(input_pins)},
      output_pins_{std::move(output_pins)},
      ui_data_{.name = std::move(name), .color = color} {}

auto Node::GetId() const -> ne::NodeId { return id_; }

auto Node::GetInputPins() const -> const std::vector<Pin>& {
  return input_pins_;
}

auto Node::GetInputPins() -> std::vector<Pin>& { return input_pins_; }

auto Node::GetOutputPins() const -> const std::vector<Pin>& {
  return output_pins_;
}

auto Node::GetOutputPins() -> std::vector<Pin>& { return output_pins_; }