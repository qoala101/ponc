#ifndef VH_CORE_NODE_H_
#define VH_CORE_NODE_H_

#include <imgui_node_editor.h>

#include <string>
#include <vector>

#include "core_pin.h"
#include "esc_enums.h"

namespace ne = ax::NodeEditor;

struct NodeUiData {
  std::string name{};
  ImColor color{255, 255, 255};
};

class Node {
 public:
  auto GetId [[nodiscard]] () const -> ne::NodeId;

  auto GetInputPins [[nodiscard]] () const -> const std::vector<Pin> &;
  auto GetInputPins [[nodiscard]] () -> std::vector<Pin> &;

  auto GetOutputPins [[nodiscard]] () const -> const std::vector<Pin> &;
  auto GetOutputPins [[nodiscard]] () -> std::vector<Pin> &;

  NodeUiData ui_data_{};

  int coupler_percentage_index_{};

 protected:
  Node(ne::NodeId id, std::vector<Pin> input_pins, std::vector<Pin> output_pins,
       std::string name, ImColor color);

 private:
  ne::NodeId id_{};
  std::vector<Pin> input_pins_{};
  std::vector<Pin> output_pins_{};
};

#endif  // VH_CORE_NODE_H_
