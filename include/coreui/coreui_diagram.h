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
class Diagram {
 public:
  ///
  struct Hooks {
    ///
    std::function<auto()->bool> is_color_flow{};
    ///
    std::function<auto(float flow)->ImColor> get_flow_color{};
  };

  Diagram(cpp::SafePointer<core::Diagram> diagram,
          cpp::SafePointer<EventLoop> event_loop, Hooks hooks);

  void OnFrame();

  auto GetLinkCreation() -> LinkCreation &;
  auto GetNodes() const -> const std::vector<Node> &;
  auto GetLinks() const -> const std::vector<Link> &;

  void Event_EmplaceNode(std::unique_ptr<core::INode> node,
                         const ImVec2 &position);
  void Event_CreateLink(ne::PinId start_pin_id, ne::PinId end_pin_id);
  void Event_DeleteLink(ne::LinkId link_id);

 private:
  void UpdateLinks(
      const std::unordered_map<uintptr_t, flow::NodeFlow> &node_flows,
      bool color_flow);
  void UpdateNodes(
      const std::unordered_map<uintptr_t, flow::NodeFlow> &node_flows,
      bool color_flow);

  auto GetPinIconAlpha(ne::PinId pin_id) const;
  auto GetFlowLinkAlpha(ne::LinkId link_id) const;

  cpp::SafePointer<core::Diagram> diagram_;
  cpp::SafePointer<EventLoop> event_loop_;
  Hooks hooks_{};

  LinkCreation link_creation_;
  std::vector<Node> nodes_{};
  std::vector<Link> links_{};
};
}  // namespace esc::coreui

#endif  // VH_COREUI_DIAGRAM_H_
