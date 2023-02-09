#ifndef VH_CORE_PIN_H_
#define VH_CORE_PIN_H_

#include <imgui_node_editor.h>
#include <string>

#include "esc_enums.h"

namespace ne = ax::NodeEditor;

struct Node;

struct Pin {
  Pin(ne::PinId id, std::string name, PinType type, PinKind kind, Node* node);

  ne::PinId ID;
  Node* node;
  std::string Name;
  PinType Type;
  PinKind Kind;

  float value{};
  bool editable{};
};

#endif  // VH_CORE_PIN_H_
