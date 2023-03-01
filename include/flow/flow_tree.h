#ifndef VH_CORE_TREE_H_
#define VH_CORE_TREE_H_

#include <concepts>
#include <functional>
#include <memory>
#include <optional>
#include <vector>

#include "core_i_node.h"
#include "cpp_safe_pointer.h"

namespace esc::flow {
///
struct TreeNode {
  ///
  cpp::SafePointer<core::INode> node;
  ///
  std::map<uintptr_t, TreeNode> child_nodes{};
};

///
struct FlowTree {
  ///
  std::vector<TreeNode> root_nodes{};
};
}  // namespace esc::flow

#endif  // VH_CORE_TREE_H_
