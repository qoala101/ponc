#ifndef VH_PONC_FLOW_TREE_H_
#define VH_PONC_FLOW_TREE_H_

#include <imgui_node_editor.h>

#include <concepts>
#include <functional>
#include <map>
#include <memory>
#include <optional>
#include <vector>

namespace ne = ax::NodeEditor;

namespace vh::ponc::flow {
///
struct TreeNode {
  ///
  ne::NodeId node_id;
  ///
  std::map<uintptr_t, TreeNode> child_nodes{};
};

///
struct FlowTree {
  ///
  std::vector<TreeNode> root_nodes{};
};
}  // namespace vh::ponc::flow

#endif  // VH_PONC_FLOW_TREE_H_
