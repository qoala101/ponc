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
  Pin(int id, const char* name, PinType type);

  ne::PinId ID;
  ::Node* Node;
  std::string Name;
  PinType Type;
  PinKind Kind;
};

auto CanCreateLink [[nodiscard]] (const Pin* left, const Pin* right) -> bool;

struct Node {
  Node(int id, const char* name, ImColor color = ImColor{255, 255, 255});

  ne::NodeId ID;
  std::string Name;
  std::vector<Pin> Inputs;
  std::vector<Pin> Outputs;
  ImColor Color;
  NodeType Type;
  ImVec2 Size;
  std::string State;
  std::string SavedState;
};

struct Link {
  // Link(ne::LinkId id, ne::PinId startPinId, ne::PinId endPinId);

  ne::LinkId ID;
  ne::PinId StartPinID;
  ne::PinId EndPinID;
  ImColor Color;
};

#endif  // VH_ESC_TYPES_H_
