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

struct Node;

struct Pin {
  ed::PinId ID;
  ::Node* Node;
  std::string Name;
  PinType Type;
  PinKind Kind;

  Pin(int id, const char* name, PinType type);
};

struct Node {
  ed::NodeId ID;
  std::string Name;
  std::vector<Pin> Inputs;
  std::vector<Pin> Outputs;
  ImColor Color;
  NodeType Type;
  ImVec2 Size;

  std::string State;
  std::string SavedState;

  Node(int id, const char* name, ImColor color = ImColor(255, 255, 255));
};

struct Link {
  ed::LinkId ID;

  ed::PinId StartPinID;
  ed::PinId EndPinID;

  ImColor Color;

  Link(ed::LinkId id, ed::PinId startPinId, ed::PinId endPinId);
};

struct NodeIdLess {
  auto operator()(const ed::NodeId& lhs, const ed::NodeId& rhs) const -> bool;
};

#endif  // VH_ESC_TYPES_H_
