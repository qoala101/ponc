#ifndef VH_CORE_NODE_H_
#define VH_CORE_NODE_H_

#include <imgui_node_editor.h>

#include <memory>
#include <string>
#include <vector>

#include "core_pin.h"
#include "ui_i_node_drawer.h"

namespace ne = ax::NodeEditor;

class Node {
 public:
  Node(const Node &) = delete;
  Node(Node &&) noexcept = delete;

  auto operator=(const Node &) noexcept -> Node & = delete;
  auto operator=(Node &&) noexcept -> Node & = delete;

  virtual ~Node() = default;

  virtual auto GetDrawer [[nodiscard]] ()
  -> std::unique_ptr<vh::esc::ui::INodeDrawer> = 0;

  auto GetId [[nodiscard]] () const -> ne::NodeId;

  auto GetInputPins [[nodiscard]] () const
      -> const std::vector<std::shared_ptr<Pin>> &;
  auto GetInputPins [[nodiscard]] () -> std::vector<std::shared_ptr<Pin>> &;

  auto GetOutputPins [[nodiscard]] () const
      -> const std::vector<std::shared_ptr<Pin>> &;
  auto GetOutputPins [[nodiscard]] () -> std::vector<std::shared_ptr<Pin>> &;

 protected:
 Node() = default;
  Node(ne::NodeId id, std::vector<std::shared_ptr<Pin>> input_pins,
       std::vector<std::shared_ptr<Pin>> output_pins);

 private:
  ne::NodeId id_{};
  std::vector<std::shared_ptr<Pin>> input_pins_{};
  std::vector<std::shared_ptr<Pin>> output_pins_{};
};

#endif  // VH_CORE_NODE_H_
