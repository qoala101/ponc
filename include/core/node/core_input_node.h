#ifndef VH_CORE_INPUT_NODE_H_
#define VH_CORE_INPUT_NODE_H_

#include "core_node.h"
#include "esc_id_generator.h"

namespace esc {
// NOLINTNEXTLINE(*-multiple-inheritance)
class InputNode : public Node, public std::enable_shared_from_this<InputNode> {
 public:
  explicit InputNode(esc::IdGenerator& id_generator);

  auto CreateDrawer [[nodiscard]] ()
  -> std::unique_ptr<INodeDrawer> override;
};

class InputNodeDrawer : public INodeDrawer {
 public:
  explicit InputNodeDrawer(std::shared_ptr<InputNode> node);

  auto GetLabel [[nodiscard]] () const -> std::string override;
  auto GetColor [[nodiscard]] () const -> ImColor override;

 private:
  std::shared_ptr<InputNode> node_{};
};
}  // namespace esc

#endif  // VH_CORE_INPUT_NODE_H_
