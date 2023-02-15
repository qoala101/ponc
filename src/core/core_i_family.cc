#include "core_i_family.h"

#include <bits/ranges_algo.h>

namespace esc::core {
// ---
auto IsChildOf(const INode &node, const IFamily &family) -> bool {
  return std::ranges::any_of(family.nodes_, [&node](const auto &family_node) {
    return &*family_node == &node;
  });
}

// ---
auto IFamily::GetNodes() const -> const std::vector<std::shared_ptr<INode>> & {
  return nodes_;
}

// ---
auto IFamily::EmplaceNode(IdGenerator &id_generator) -> INode & {
  return EmplaceNode(CreateNode(id_generator));
}

// ---
void IFamily::EraseNode(ne::NodeId &id) {
  nodes_.erase(std::ranges::find_if(
      nodes_, [id](const auto &node) { return node->GetId() == id; }));
}

// ---
IFamily::IFamily(std::vector<std::shared_ptr<core::INode>> nodes)
    : nodes_{std::move(nodes)} {}

// ---
auto IFamily::EmplaceNode(std::shared_ptr<core::INode> node) -> INode & {
  return *nodes_.emplace_back(std::move(node));
}
}  // namespace esc::core