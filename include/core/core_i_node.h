/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_CORE_I_NODE_H_
#define VH_PONC_CORE_I_NODE_H_

#include <imgui.h>
#include <imgui_node_editor.h>

#include <memory>
#include <optional>
#include <utility>
#include <vector>

#include "core_i_family.h"
#include "core_id_ptr.h"
#include "core_id_value.h"
#include "cpp_non_copyable.h"
#include "flow_node_flow.h"

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
struct NodeIds {
  ///
  ne::NodeId *node_id{};
  ///
  std::optional<ne::PinId *> input_pin_id{};
  ///
  std::vector<ne::PinId *> output_pin_ids{};
};

///
class INode : public cpp::NonCopyable {
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
  static auto GetIds(const INode &node) -> std::vector<UnspecifiedIdValue>;
  ///
  static auto GetIds(INode &node) -> std::vector<IdPtr>;
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
  auto GetIds() -> NodeIds;
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
