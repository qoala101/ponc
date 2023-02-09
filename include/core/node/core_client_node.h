#ifndef VH_CORE_CLIENT_NODE_H_
#define VH_CORE_CLIENT_NODE_H_

#include <memory>

#include "core_node.h"
#include "esc_id_generator.h"

// NOLINTNEXTLINE(*-multiple-inheritance)
class ClientNode : public Node,
                   public std::enable_shared_from_this<ClientNode> {
 public:
  explicit ClientNode(esc::IdGenerator& id_generator);

  auto GetDrawer [[nodiscard]] ()
  -> std::unique_ptr<vh::esc::ui::INodeDrawer> override;
};

#endif  // VH_CORE_CLIENT_NODE_H_
