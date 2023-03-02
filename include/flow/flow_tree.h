#ifndef VH_FLOW_TREE_H_
#define VH_FLOW_TREE_H_

#include <concepts>
#include <functional>
#include <memory>
#include <optional>
#include <vector>

#include "core_i_node.h"
#include "cpp_safe_ptr.h"

namespace esc::flow {
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
}  // namespace esc::flow

#endif  // VH_FLOW_TREE_H_
