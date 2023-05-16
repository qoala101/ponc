#ifndef VH_PONC_COREUI_NATIVE_FACADE_H_
#define VH_PONC_COREUI_NATIVE_FACADE_H_

#include <imgui_node_editor.h>

#include <vector>

#include "cpp_static_api.h"

namespace ne = ax::NodeEditor;

namespace vh::ponc::coreui {
///
struct NativeFacade : public cpp::StaticApi {
  ///
  static auto GetSelectedNodes() -> std::vector<ne::NodeId>;
  ///
  static auto GetSelectedLinks() -> std::vector<ne::LinkId>;
  ///
  static void SelectNode(ne::NodeId node_id, bool append = false);
  ///
  static void SelectLink(ne::LinkId link_id, bool append = false);
};
}  // namespace vh::ponc::coreui

#endif  // VH_PONC_COREUI_NATIVE_FACADE_H_
