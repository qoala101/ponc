#ifndef VH_CORE_PIN_H_
#define VH_CORE_PIN_H_

#include <imgui_node_editor.h>

#include "cpp_no_instances.h"

namespace ne = ax::NodeEditor;

namespace esc::core {
//
struct Pin : cpp::NoInstances {
  ///
  static auto GetOppositeKind(ne::PinKind pin_kind) -> ne::PinKind;
};
}  // namespace esc::core

#endif  // VH_CORE_PIN_H_
