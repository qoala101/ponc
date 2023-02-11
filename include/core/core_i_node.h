#ifndef VH_CORE_I_NODE_H_
#define VH_CORE_I_NODE_H_

#include <imgui_node_editor.h>

#include <memory>
#include <string>
#include <vector>

#include "cpp_interface.h"
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
class INode : public cpp::Interface {
 public:
  // ADD ID REGISTERING AS RAII
  // virtual ~INode() = default;

  // ---
  virtual auto CreateWriter [[nodiscard]] ()
  -> std::unique_ptr<json::INodeWriter> = 0;
  // ---
  virtual auto CreateDrawer [[nodiscard]] ()
  -> std::unique_ptr<draw::INodeDrawer> = 0;

  // ---
  auto GetId [[nodiscard]] () const -> ne::NodeId;
  // ---
  auto GetPinIds [[nodiscard]] () const -> const std::vector<ne::PinId> &;
  // ---
  auto GetPosition [[nodiscard]] () const -> ImVec2;
  // ---
  void SetPosition(const ImVec2 &position);

 protected:
  // ---
  INode(ne::NodeId id, std::vector<ne::PinId> pin_ids);

 private:
  ne::NodeId id_{};
  std::vector<ne::PinId> pin_ids_{};
};
}  // namespace core
}  // namespace esc

#endif  // VH_CORE_I_NODE_H_
