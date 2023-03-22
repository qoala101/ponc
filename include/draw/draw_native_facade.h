#ifndef VH_PONC_DRAW_NATIVE_FACADE_H_
#define VH_PONC_DRAW_NATIVE_FACADE_H_

#include <imgui_node_editor.h>

#include <vector>

#include "cpp_no_instances.h"

namespace ne = ax::NodeEditor;

namespace vh::ponc::draw {
///
struct NativeFacade : public cpp::NoInstances {
  ///
  static auto GetSelectedNodes() -> std::vector<ne::NodeId>;
  ///
  static auto GetSelectedLinks() -> std::vector<ne::LinkId>;
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_NATIVE_FACADE_H_
