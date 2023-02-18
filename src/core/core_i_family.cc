/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#include "core_i_family.h"

#include <algorithm>
#include <ranges>

#include "cpp_assert.h"

namespace esc::core {
// ---
auto IFamily::GetNodes() const -> const std::vector<std::shared_ptr<INode>> & {
  return nodes_;
}

// ---
auto IFamily::FindNode(ne::NodeId node_id) const
    -> const std::shared_ptr<INode> * {
  const auto found_node = std::ranges::find_if(
      nodes_, [node_id](const auto &node) { return node->GetId() == node_id; });

  if (found_node == nodes_.end()) {
    return nullptr;
  }

  return &*found_node;
}

// ---
auto IFamily::EmplaceNode(IdGenerator &id_generator)
    -> const std::shared_ptr<INode> & {
  return EmplaceNode(CreateNode(id_generator));
}

// ---
void IFamily::EraseNode(ne::NodeId node_id) {
  const auto found_node = std::ranges::find_if(
      nodes_, [node_id](const auto &node) { return node->GetId() == node_id; });

  Expects(found_node != nodes_.end());
  nodes_.erase(found_node);
}

// ---
IFamily::IFamily(std::vector<std::shared_ptr<core::INode>> nodes)
    : nodes_{std::move(nodes)} {}

// ---
auto IFamily::EmplaceNode(std::shared_ptr<core::INode> node)
    -> const std::shared_ptr<INode> & {
  return nodes_.emplace_back(std::move(node));
}

// ---
auto IsChildOf(ne::NodeId node_id, const IFamily &family) -> bool {
  return family.FindNode(node_id) != nullptr;
}
}  // namespace esc::core