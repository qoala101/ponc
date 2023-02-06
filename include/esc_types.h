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

namespace ed = ax::NodeEditor;

enum class PinType {
  Flow,
  Bool,
  Int,
  Float,
  String,
  Object,
  Function,
  Delegate,
};

enum class PinKind { Output, Input };

enum class NodeType { Blueprint, Simple, Tree, Comment, Houdini };

namespace ax::Drawing {
enum class IconType : ImU32 {
  Flow,
  Circle,
  Square,
  Grid,
  RoundSquare,
  Diamond
};
}  // namespace ax::Drawing

struct Node;

struct Pin {
  Pin(int id, const char* name, PinType type);

  ed::PinId ID;
  ::Node* Node;
  std::string Name;
  PinType Type;
  PinKind Kind;
};

struct Node {
  Node(int id, const char* name, ImColor color = ImColor{255, 255, 255});

  ed::NodeId ID;
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
  Link(ed::LinkId id, ed::PinId startPinId, ed::PinId endPinId);

  ed::LinkId ID;
  ed::PinId StartPinID;
  ed::PinId EndPinID;
  ImColor Color;
};

struct NodeIdLess {
  auto operator()(const ed::NodeId& lhs, const ed::NodeId& rhs) const -> bool;
};

#endif  // VH_ESC_TYPES_H_
