#ifndef VH_CORE_NODE_H_
#define VH_CORE_NODE_H_

#include <imgui_node_editor.h>

#include <memory>
#include <string>
#include <vector>

#include "core_pin.h"
#include "esc_id_generator.h"

namespace ne = ax::NodeEditor;

namespace esc {
// NOLINTNEXTLINE(*-special-member-functions)
class INodeDrawer {
 public:
  virtual ~INodeDrawer() = default;

  virtual auto GetLabel() const -> std::string = 0;
  virtual auto GetColor() const -> ImColor = 0;

 protected:
  INodeDrawer() = default;
};
}  // namespace esc

class INode {
 public:
  INode(const INode &) = delete;
  INode(INode &&) noexcept = delete;

  auto operator=(const INode &) noexcept -> INode & = delete;
  auto operator=(INode &&) noexcept -> INode & = delete;

  virtual ~INode() = default;

  virtual auto CreateDrawer() -> std::unique_ptr<esc::INodeDrawer> = 0;

  auto GetId() const -> ne::NodeId;

  auto GetInputPins() const -> const std::vector<std::shared_ptr<IPin>> &;
  auto GetInputPins() -> std::vector<std::shared_ptr<IPin>> &;

  auto GetOutputPins() const -> const std::vector<std::shared_ptr<IPin>> &;
  auto GetOutputPins() -> std::vector<std::shared_ptr<IPin>> &;

 protected:
  INode(ne::NodeId id, std::vector<std::shared_ptr<IPin>> input_pins,
       std::vector<std::shared_ptr<IPin>> output_pins);

 private:
  ne::NodeId id_{};
  std::vector<std::shared_ptr<IPin>> input_pins_{};
  std::vector<std::shared_ptr<IPin>> output_pins_{};
};

namespace esc {
// NOLINTNEXTLINE(*-special-member-functions)
class INodeFactoryDrawer {
 public:
  virtual ~INodeFactoryDrawer() = default;

  virtual auto GetLabel() const -> std::string = 0;
  virtual auto GetColor() const -> ImColor = 0;

 protected:
  INodeFactoryDrawer() = default;
};

// NOLINTNEXTLINE(*-special-member-functions)
class INodeFactory {
 public:
  virtual ~INodeFactory() = default;

  virtual auto CreateNode(IdGenerator &id_generator) -> std::shared_ptr<INode> = 0;
  virtual auto CreateDrawer() -> std::unique_ptr<INodeFactoryDrawer> = 0;

 protected:
  INodeFactory() = default;
};
}  // namespace esc

#endif  // VH_CORE_NODE_H_
