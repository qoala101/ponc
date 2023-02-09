#ifndef VH_UI_CLIENT_NODE_DRAWER_H_
#define VH_UI_CLIENT_NODE_DRAWER_H_

#include <memory>

#include "core_client_node.h"
#include "ui_i_node_drawer.h"

namespace vh::esc::ui {
class ClientNodeDrawer : public INodeDrawer {
 public:
  explicit ClientNodeDrawer(std::shared_ptr<ClientNode> node);

  auto GetName [[nodiscard]] () const -> std::string override;
  auto GetColor [[nodiscard]] () const -> ImColor override;

 private:
  std::shared_ptr<ClientNode> node_{};
};
}  // namespace vh::esc::ui

#endif  // VH_UI_CLIENT_NODE_DRAWER_H_
