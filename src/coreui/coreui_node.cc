#include "coreui_node.h"

namespace esc::coreui {
///
Node::Node(cpp::SafePtr<core::INode> core_node, NodeData data)
    : core_node_{std::move(core_node)}, data_{std::move(data)} {}

///
auto Node::GetNode() const -> core::INode& { return *core_node_; }

///
auto Node::GetData() const -> const NodeData& { return data_; }
}  // namespace esc::coreui