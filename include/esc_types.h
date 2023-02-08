#ifndef VH_ESC_TYPES_H_
#define VH_ESC_TYPES_H_

#include <application.h>
#include <imgui_node_editor.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include <algorithm>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "esc_enums.h"

namespace ne = ax::NodeEditor;

struct Node;

struct Pin {
  Pin(int id, const char* name, PinType type, PinKind kind, Node* node);

  ne::PinId ID;
  Node* node;
  std::string Name;
  PinType Type;
  PinKind Kind;

  float value{};
  bool editable{};
};

auto CanCreateLink [[nodiscard]] (const Pin* left, const Pin* right) -> bool;

struct Node {
  Node(int id, const char* name, ImColor color = ImColor{255, 255, 255});

  ne::NodeId ID{};
  std::string Name{};
  std::vector<Pin> Inputs{};
  std::vector<Pin> Outputs{};
  ImColor Color{255, 255, 255};
  NodeType Type{NodeType::Blueprint};
  ImVec2 Size{0, 0};

  int coupler_percentage_index_{};
};

auto GetCouplerPercentageNames [[nodiscard]] () -> const std::vector<std::string> &;
auto GetCouplerPercentageValues [[nodiscard]] () -> const std::vector<std::pair<float, float>> &;

struct Link {
  ne::LinkId ID{};
  ne::PinId StartPinID{};
  ne::PinId EndPinID{};
  ImColor Color{255, 255, 255};
};

#endif  // VH_ESC_TYPES_H_
