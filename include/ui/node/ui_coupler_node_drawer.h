#ifndef VH_UI_COUPLER_NODE_DRAWER_H_
#define VH_UI_COUPLER_NODE_DRAWER_H_

#include <memory>

#include "core_coupler_node.h"
#include "ui_i_node_drawer.h"

namespace vh::esc::ui {
class CouplerNodeDrawer : public INodeDrawer {
 public:
  explicit CouplerNodeDrawer(std::shared_ptr<CouplerNode> node);

  auto GetName [[nodiscard]] () const -> std::string override;
  auto GetColor [[nodiscard]] () const -> ImColor override;

 private:
  std::shared_ptr<CouplerNode> node_{};
};
}  // namespace vh::esc::ui

#endif  // VH_UI_COUPLER_NODE_DRAWER_H_
