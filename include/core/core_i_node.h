#ifndef VH_PONC_CORE_I_NODE_H_
#define VH_PONC_CORE_I_NODE_H_

#include <imgui_node_editor.h>

#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

#include "core_i_family.h"
#include "core_id_ptr.h"
#include "cpp_interface.h"
#include "flow_node_flow.h"
#include "imgui.h"

namespace ne = ax::NodeEditor;

namespace vh::ponc {
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
  static auto GetAllPins(const INode &node)
      -> std::vector<std::pair<ne::PinId, ne::PinKind>>;
  ///
  static auto GetPinKind(const INode &node, ne::PinId pin_id) -> ne::PinKind;
  ///
  static auto GetFirstPinOfKind(const INode &node, ne::PinKind pin_kind)
      -> ne::PinId;
  ///
  static auto FindFirstPinOfKind(const INode &node, ne::PinKind pin_kind)
      -> std::optional<ne::PinId>;

  ///
  virtual auto CreateWriter() const -> std::unique_ptr<json::INodeWriter>;
  ///
  virtual auto CreateUiTraits() -> std::unique_ptr<coreui::INodeTraits> = 0;

  ///
  auto GetId() const -> ne::NodeId;
  ///
  auto GetIds() -> std::vector<IdPtr>;
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
  explicit INode(const ConstructorArgs &args);

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
}  // namespace vh::ponc

#endif  // VH_PONC_CORE_I_NODE_H_
