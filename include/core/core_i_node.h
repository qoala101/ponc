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
  // ---
  struct ConstructorArgs {
    // ---
    ne::NodeId id{};
    // ---
    FamilyId family_id{};
    // ---
    ne::PinId input_pin_id{};
    // ---
    std::vector<ne::PinId> output_pin_ids{};
  };

  // ---
  virtual auto CreateWriter [[nodiscard]] ()
  -> std::unique_ptr<json::INodeWriter> = 0;

  virtual auto CreateDrawer [[nodiscard]] ()
  -> std::unique_ptr<coreui::INodeDrawer> = 0;

  // ---
  auto GetId [[nodiscard]] () const -> ne::NodeId;
  // ---
  auto GetFamilyId [[nodiscard]] () const -> FamilyId;
  // ---
  auto GetInputPinId [[nodiscard]] () const -> const std::optional<ne::PinId> &;
  // ---
  auto GetOutputPinIds [[nodiscard]] () const -> const std::vector<ne::PinId> &;
  // ---
  auto GetPosition [[nodiscard]] () const -> ImVec2;
  // ---
  void SetPosition(const ImVec2 &position);
  // ---
  auto GetInitialFlow [[nodiscard]] () const -> flow::NodeFlow;

 protected:
  // ---
  explicit INode(ConstructorArgs args);

 private:
  // ---
  virtual void SetInitialFlowValues(flow::NodeFlow &node_flow) const;

  // ---
  ne::NodeId id_{};
  // ---
  FamilyId family_id_{};
  // ---
  std::optional<ne::PinId> input_pin_id_{};
  // ---
  std::vector<ne::PinId> output_pin_ids_{};
};

// ---
auto GetAllPinIds [[nodiscard]] (const INode &node) -> std::vector<ne::PinId>;

// ---
auto GetPinKind [[nodiscard]] (const INode &node, ne::PinId pin_id)
-> ne::PinKind;
}  // namespace core
}  // namespace esc

#endif  // VH_CORE_I_NODE_H_
