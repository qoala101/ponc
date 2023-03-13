#include "coreui_node.h"

namespace esc::coreui {
///
Node::Node(cpp::SafePtr<core::INode> node, NodeData data)
    : node_{std::move(node)}, data_{std::move(data)} {}

///
auto Node::GetNode() const -> core::INode& { return *node_; }

///
auto Node::GetData() const -> const NodeData& { return data_; }
}  // namespace esc::coreui