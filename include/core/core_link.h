#ifndef VH_CORE_LINK_H_
#define VH_CORE_LINK_H_

#include <imgui_node_editor.h>

namespace esc::core {
namespace ne = ax::NodeEditor;

struct Link {
  ne::LinkId id{};
  ne::PinId start_pin_id{};
  ne::PinId end_pin_id{};
};
}  // namespace esc::core

#endif  // VH_CORE_LINK_H_
