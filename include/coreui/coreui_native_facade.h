/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

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
  static auto GetSelectedNodesAndAreas()
      -> std::pair<std::vector<ne::NodeId>, std::vector<ne::NodeId>>;
  ///
  static auto GetSelectedLinks() -> std::vector<ne::LinkId>;
  ///
  static void SelectNode(ne::NodeId node_id, bool append = false);
  ///
  static void SelectLink(ne::LinkId link_id, bool append = false);
};
}  // namespace vh::ponc::coreui

#endif  // VH_PONC_COREUI_NATIVE_FACADE_H_
