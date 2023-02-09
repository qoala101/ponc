#ifndef VH_CORE_INPUT_NODE_H_
#define VH_CORE_INPUT_NODE_H_

#include "core_node.h"
#include "esc_id_generator.h"

// NOLINTNEXTLINE(*-multiple-inheritance)
class InputNode : public Node, public std::enable_shared_from_this<InputNode> {
 public:
  explicit InputNode(esc::IdGenerator& id_generator);

  auto GetDrawer [[nodiscard]] ()
  -> std::unique_ptr<vh::esc::ui::INodeDrawer> override;
};

#endif  // VH_CORE_INPUT_NODE_H_
