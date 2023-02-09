#ifndef VH_UI_INPUT_NODE_H_
#define VH_UI_INPUT_NODE_H_

#include <memory>

#include "core_input_node.h"
#include "ui_i_node_drawer.h"

namespace vh::esc::ui {
class InputNodeDrawer : public INodeDrawer {
 public:
  explicit InputNodeDrawer(std::shared_ptr<InputNode> node);

  auto GetName [[nodiscard]] () const -> std::string override;
  auto GetColor [[nodiscard]] () const -> ImColor override;

 private:
  std::shared_ptr<InputNode> node_{};
};
}  // namespace vh::esc::ui

#endif  // VH_UI_INPUT_NODE_H_
