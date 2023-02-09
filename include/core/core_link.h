#ifndef VH_CORE_LINK_H_
#define VH_CORE_LINK_H_

#include <imgui_node_editor.h>

namespace ne = ax::NodeEditor;

struct Link {
  ne::LinkId ID{};
  ne::PinId StartPinID{};
  ne::PinId EndPinID{};
  ImColor Color{255, 255, 255};
};

#endif  // VH_CORE_LINK_H_
