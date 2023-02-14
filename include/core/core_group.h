#ifndef VH_CORE_GROUP_H_
#define VH_CORE_GROUP_H_

#include <memory>

#include "core_i_node.h"

namespace esc::core {
class Group {
 public:
  explicit Group(const std::vector<std::shared_ptr<INode>> &nodes = {});

  auto GetName() const -> const std::string &;
  void SetName(std::string name);

  auto GetNodes() const -> std::vector<std::shared_ptr<INode>>;

 private:
  std::string name_{};
  std::vector<std::weak_ptr<INode>> nodes_{};
};
}  // namespace esc::core

#endif  // VH_CORE_GROUP_H_
