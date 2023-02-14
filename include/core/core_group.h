#ifndef VH_CORE_GROUP_H_
#define VH_CORE_GROUP_H_

#include <memory>

#include "core_i_node.h"
#include "imgui.h"

namespace esc::core {
class Group {
 public:
  explicit Group(const std::vector<std::shared_ptr<INode>> &nodes = {});

  auto GetNodes() const -> std::vector<std::shared_ptr<INode>>;

  std::string name_{};
  std::array<float, 3> color_{};
  bool fill_background_{};
  bool unite_{};

 private:
  std::vector<std::weak_ptr<INode>> nodes_{};
};
}  // namespace esc::core

#endif  // VH_CORE_GROUP_H_
