/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#ifndef VH_CORE_I_NODE_H_
#define VH_CORE_I_NODE_H_

#include <imgui_node_editor.h>

#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

#include "core_family_id.h"
#include "cpp_interface.h"
#include "flow_node_flow.h"
#include "imgui.h"

namespace ne = ax::NodeEditor;

namespace esc {
namespace json {
class INodeWriter;
}  // namespace json

namespace coreui {
class INodeDrawer;
}  // namespace coreui

namespace core {
class INode : public cpp::Interface {
 public:
  struct ConstructorArgs {
    ne::NodeId id{};
    FamilyId family_id{};
    std::optional<ne::PinId> input_pin_id{};
    std::vector<ne::PinId> output_pin_ids{};
  };

  virtual auto CreateWriter() -> std::unique_ptr<json::INodeWriter> = 0;

  virtual auto CreateDrawer() -> std::unique_ptr<coreui::INodeDrawer> = 0;

  auto GetId() const -> ne::NodeId;
  auto GetFamilyId() const -> FamilyId;
  auto GetInputPinId() const -> const std::optional<ne::PinId> &;
  auto GetOutputPinIds() const -> const std::vector<ne::PinId> &;
  auto GetPosition() const -> ImVec2;
  void SetPosition(const ImVec2 &position);
  auto GetInitialFlow() const -> flow::NodeFlow;

 protected:
  explicit INode(ConstructorArgs args);

 private:
  virtual void SetInitialFlowValues(flow::NodeFlow &node_flow) const;

  ne::NodeId id_{};
  FamilyId family_id_{};
  std::optional<ne::PinId> input_pin_id_{};
  std::vector<ne::PinId> output_pin_ids_{};
};

auto GetAllPinIds(const INode &node) -> std::vector<ne::PinId>;

auto GetPinKind(const INode &node, ne::PinId pin_id) -> ne::PinKind;
}  // namespace core
}  // namespace esc

#endif  // VH_CORE_I_NODE_H_
