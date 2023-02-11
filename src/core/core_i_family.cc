#include "core_i_family.h"

#include <bits/ranges_algo.h>

namespace esc::core {
IFamily::IFamily(std::vector<std::shared_ptr<core::INode>> nodes)
    : nodes_{std::move(nodes)} {}

auto IFamily::GetNodes() const -> const std::vector<std::shared_ptr<INode>> & {
  return nodes_;
}

auto IFamily::EmplaceNode(IdGenerator &id_generator) -> INode & {
  return *nodes_.emplace_back(CreateNode(id_generator));
}

void IFamily::EraseNode(ne::NodeId &id) {
  nodes_.erase(std::ranges::find_if(
      nodes_, [id](const auto &node) { return node->GetId() == id; }));
}
}  // namespace esc::core