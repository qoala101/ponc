#ifndef VH_COREUI_DIAGRAM_H_
#define VH_COREUI_DIAGRAM_H_

#include <imgui_node_editor.h>

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "core_diagram.h"
#include "core_i_family.h"
#include "core_i_node.h"
#include "core_id_generator.h"
#include "core_link.h"
#include "core_project.h"
#include "coreui_event_loop.h"
#include "coreui_i_pin_traits.h"
#include "coreui_link.h"
#include "coreui_link_creation.h"
#include "coreui_node.h"
#include "coreui_texture.h"
#include "cpp_safe_pointer.h"
#include "imgui.h"
#include "imgui_internal.h"

namespace esc::coreui {
///
class Diagram {
 public:
  ///
  struct Hooks {
    ///
    std::function<auto()->bool> is_color_flow{};
    ///
    std::function<auto(float flow)->ImColor> get_flow_color{};
  };

  ///
  Diagram(cpp::SafePointer<core::Diagram> diagram,
          cpp::SafePointer<EventLoop> event_loop, Hooks hooks);

  ///
  void OnFrame();

  ///
  auto GetLinkCreation() -> LinkCreation &;
  ///
  auto GetNodes() const -> const std::vector<Node> &;
  ///
  auto GetLinks() const -> const std::vector<Link> &;

 private:
  ///
  auto GetFlowLinkAlpha(ne::LinkId link_id) const;
  ///
  auto FlowLinkFrom(const core::Link &core_link,
                    const flow::NodeFlows &node_flows) const;
  ///
  auto GetPinIconTipPos(ne::PinId pin_id) const;
  ///
  auto GetRepinningLinkColor() const;
  ///
  auto GetRepinningLink() const -> std::optional<Link>;
  ///
  void UpdateLinks(const flow::NodeFlows &node_flows);

  ///
  auto GetNodeHeaderColor(const INodeTraits &node_traits,
                          const flow::NodeFlow &node_flow) const;
  ///
  auto GetPinIconAlpha(ne::PinId pin_id) const;
  ///
  auto PinFrom(const IPinTraits &pin_traits,
               const flow::NodeFlow &node_flow) const;
  ///
  auto NodeFrom(const core::INode &core_node,
                const flow::NodeFlow &node_flow) const;
  ///
  void UpdateNodes(const flow::NodeFlows &node_flows);

  ///
  cpp::SafePointer<core::Diagram> diagram_;
  ///
  cpp::SafePointer<EventLoop> event_loop_;
  ///
  Hooks hooks_{};

  ///
  LinkCreation link_creation_;
  ///
  std::vector<Node> nodes_{};
  ///
  std::vector<Link> links_{};
};
}  // namespace esc::coreui

#endif  // VH_COREUI_DIAGRAM_H_
