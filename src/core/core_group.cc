#include "core_group.h"

namespace esc::core {
Group::Group(const std::vector<std::shared_ptr<INode>> &nodes)
    : nodes_{nodes.begin(), nodes.end()} {}

auto Group::GetName() const -> const std::string & { return name_; }

void Group::SetName(std::string name) { name_ = std::move(name); }

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