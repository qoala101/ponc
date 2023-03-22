#ifndef VH_PONC_FLOW_TREE_H_
#define VH_PONC_FLOW_TREE_H_

#include <concepts>
#include <functional>
#include <memory>
#include <optional>
#include <vector>

#include "core_i_node.h"
#include "cpp_safe_ptr.h"

namespace vh::ponc::flow {
///
struct TreeNode {
  ///
  cpp::SafePtr<core::INode> node;
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
