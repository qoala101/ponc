#ifndef VH_CORE_CLIENT_NODE_H_
#define VH_CORE_CLIENT_NODE_H_

#include <memory>

#include "core_node.h"
#include "esc_id_generator.h"

namespace esc {
// NOLINTNEXTLINE(*-multiple-inheritance)
class ClientNode : public Node,
                   public std::enable_shared_from_this<ClientNode> {
 public:
  explicit ClientNode(IdGenerator& id_generator);

  auto CreateDrawer [[nodiscard]] ()
  -> std::unique_ptr<INodeDrawer> override;
};

class ClientNodeDrawer : public INodeDrawer {
 public:
  explicit ClientNodeDrawer(std::shared_ptr<ClientNode> node);

  auto GetLabel [[nodiscard]] () const -> std::string override;
  auto GetColor [[nodiscard]] () const -> ImColor override;

 private:
  std::shared_ptr<ClientNode> node_{};
};
}  // namespace esc

#endif  // VH_CORE_CLIENT_NODE_H_
