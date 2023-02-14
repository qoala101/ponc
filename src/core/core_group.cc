#include "core_group.h"

namespace esc::core {
Group::Group(const std::vector<std::shared_ptr<INode>> &nodes)
    : nodes_{nodes.begin(), nodes.end()}, color_{127, 127, 127}, unite_{true} {}

auto Group::GetNodes() const -> std::vector<std::shared_ptr<INode>> {
  auto nodes = std::vector<std::shared_ptr<INode>>{};

  for (const auto &node : nodes_) {
    if (auto lock = node.lock()) {
      nodes.emplace_back(std::move(lock));
    }
  }

  return nodes;
}
}  // namespace esc::core