#ifndef VH_CORE_I_NODE_H_
#define VH_CORE_I_NODE_H_

#include <imgui_node_editor.h>

#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

#include "core_i_family.h"
#include "cpp_interface.h"
#include "flow_node_flow.h"
#include "imgui.h"

namespace ne = ax::NodeEditor;

namespace esc {
namespace json {
///
class INodeWriter;
}  // namespace json

namespace coreui {
///
class INodeTraits;
}  // namespace coreui

namespace core {
///
class INode : public cpp::Interface {
 public:
  ///
  struct ConstructorArgs {
    ///
    ne::NodeId id{};
    ///
    FamilyId family_id{};
    ///
    std::optional<ne::PinId> input_pin_id{};
    ///
    std::vector<ne::PinId> output_pin_ids{};
    ///
    ImVec2 pos{};
  };

  ///
  static auto GetAllPinIds(const INode &node) -> std::vector<ne::PinId>;
  ///
  static auto GetPinKind(const INode &node, ne::PinId pin_id) -> ne::PinKind;

  ///
  virtual auto CreateWriter() const -> std::unique_ptr<json::INodeWriter> = 0;
  ///
  virtual auto CreateUiTraits() const
      -> std::unique_ptr<coreui::INodeTraits> = 0;

  ///
  auto GetId() const -> ne::NodeId;
  ///
  auto GetFamilyId() const -> FamilyId;
  ///
  auto GetInputPinId() const -> const std::optional<ne::PinId> &;
  ///
  auto GetOutputPinIds() const -> const std::vector<ne::PinId> &;
  ///
  auto GetPos() const -> const ImVec2 &;
  ///
  void SetPos(const ImVec2 &pos);
  ///
  auto GetInitialFlow() const -> flow::NodeFlow;

 protected:
  ///
  explicit INode(ConstructorArgs args);

 private:
  ///
  virtual void SetInitialFlowValues(flow::NodeFlow &node_flow) const;

  ///
  ne::NodeId id_{};
  ///
  FamilyId family_id_{};
  ///
  std::optional<ne::PinId> input_pin_id_{};
  ///
  std::vector<ne::PinId> output_pin_ids_{};
  ///
  ImVec2 pos_{};
};
}  // namespace core
}  // namespace esc

#endif  // VH_CORE_I_NODE_H_
