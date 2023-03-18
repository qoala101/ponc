#ifndef VH_DRAW_SELECTOR_H_
#define VH_DRAW_SELECTOR_H_

#include <imgui_node_editor.h>

#include <vector>

#include "cpp_no_instances.h"

namespace ne = ax::NodeEditor;

namespace esc::draw {
///
struct NativeFacade : public cpp::NoInstances {
  ///
  static auto GetSelectedNodes() -> std::vector<ne::NodeId>;
  ///
  static auto GetSelectedLinks() -> std::vector<ne::LinkId>;
};
}  // namespace esc::draw

#endif  // VH_DRAW_SELECTOR_H_
