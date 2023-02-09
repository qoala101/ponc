#ifndef VH_CORE_NODE_H_
#define VH_CORE_NODE_H_

#include <imgui_node_editor.h>

#include <string>
#include <vector>

#include "esc_enums.h"
#include "core_pin.h"

namespace ne = ax::NodeEditor;

struct Node {
  Node(ne::NodeId id, std::string name, ImColor color);

  ne::NodeId ID{};
  std::string Name{};
  std::vector<Pin> Inputs{};
  std::vector<Pin> Outputs{};
  ImColor Color{255, 255, 255};
  NodeType Type{NodeType::Blueprint};
  ImVec2 Size{0, 0};

  int coupler_percentage_index_{};
};

#endif  // VH_CORE_NODE_H_
