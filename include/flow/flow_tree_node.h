#ifndef VH_PONC_FLOW_TREE_H_
#define VH_PONC_FLOW_TREE_H_

#include <imgui_node_editor.h>

#include <concepts>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <vector>

#include "core_id_value.h"

namespace ne = ax::NodeEditor;

namespace vh::ponc::flow {
///
struct TreeNode {
  ///
  ne::NodeId node_id{};
  ///
  std::map<core::IdValue<ne::PinId>, TreeNode> child_nodes{};

 private:
  ///
  friend auto operator==(const TreeNode &, const TreeNode &) -> bool = default;
};
}  // namespace vh::ponc::flow

#endif  // VH_PONC_FLOW_TREE_H_
