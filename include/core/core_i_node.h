#ifndef VH_CORE_I_NODE_H_
#define VH_CORE_I_NODE_H_

#include <imgui_node_editor.h>

#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

#include "cpp_interface.h"
#include "flow_node_flow.h"
#include "imgui.h"

namespace ne = ax::NodeEditor;

namespace esc {
class State;
class StateNoQueue;

namespace json {
class INodeWriter;
}  // namespace json

namespace coreui {
class INodeDrawer;
}  // namespace coreui

namespace core {
class INode : public cpp::Interface {
 public:
  // ---
  virtual auto GetInitialFlow [[nodiscard]] () const -> flow::NodeFlow = 0;
  // ---
  virtual auto CreateWriter [[nodiscard]] ()
  -> std::unique_ptr<json::INodeWriter> = 0;
  // ---
  virtual auto CreateDrawer [[nodiscard]] (const StateNoQueue &state)
  -> std::unique_ptr<coreui::INodeDrawer> = 0;

  // ---
  auto GetId [[nodiscard]] () const -> ne::NodeId;
  // ---
  auto GetPinIds [[nodiscard]] () const -> const std::vector<ne::PinId> &;
  // ---
  auto GetPinIndex [[nodiscard]] (ne::PinId pin_id) const -> int;
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
