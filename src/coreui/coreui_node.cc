/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "coreui_node.h"

#include "cpp_assert.h"
#include "imgui.h"

namespace vh::ponc::coreui {
///
Node::Node(cpp::SafePtr<core::INode> node, NodeData data)
    : node_{std::move(node)}, data_{std::move(data)} {}

///
auto Node::GetNode() const -> core::INode& { return *node_; }

///
auto Node::GetTreeNode() const -> const TreeNode& {
  Expects(tree_node_.has_value());
  return **tree_node_;
}

///
void Node::SetTreeNode(cpp::SafePtr<const TreeNode> tree_node) {
  tree_node_ = std::move(tree_node);
}

///
auto Node::GetData() const -> const NodeData& { return data_; }
}  // namespace vh::ponc::coreui