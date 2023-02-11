#ifndef VH_CORE_NODE_H_
#define VH_CORE_NODE_H_

#include <imgui_node_editor.h>

#include <memory>
#include <string>
#include <vector>

#include "core_pin.h"
#include "imgui.h"

namespace ne = ax::NodeEditor;

namespace esc {
namespace json {
class INodeWriter;
}  // namespace json

namespace draw {
class INodeDrawer;
}  // namespace draw

namespace core {
class INode {
 public:
  INode(const INode &) = delete;
  INode(INode &&) noexcept = delete;

  auto operator=(const INode &) noexcept -> INode & = delete;
  auto operator=(INode &&) noexcept -> INode & = delete;

  // ADD ID REGISTERING AS RAII
  virtual ~INode() = default;

  virtual auto CreateWriter() -> std::unique_ptr<json::INodeWriter> = 0;
  virtual auto CreateDrawer() -> std::unique_ptr<draw::INodeDrawer> = 0;

  auto GetId() const -> ne::NodeId;
  auto GetPosition() const -> ImVec2;

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
}  // namespace core
}  // namespace esc

#endif  // VH_CORE_NODE_H_
