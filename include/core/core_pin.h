#ifndef VH_CORE_PIN_H_
#define VH_CORE_PIN_H_

#include <imgui_node_editor.h>

#include <string>

#include "esc_enums.h"

namespace ne = ax::NodeEditor;

class Node;

class Pin {
 public:
  Pin(ne::PinId id, std::string name, PinType type, PinKind kind,
      bool editable);

  ne::PinId ID;
  PinType Type;
  PinKind Kind;

  float value{};

  struct {
    std::string Name{};
    bool editable{};
    Node* node;
  } ui_data_{};
};

#endif  // VH_CORE_PIN_H_
