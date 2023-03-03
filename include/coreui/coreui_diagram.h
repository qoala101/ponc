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
#include "coreui_family.h"
#include "coreui_i_header_traits.h"
#include "coreui_i_pin_traits.h"
#include "coreui_link.h"
#include "coreui_link_creation.h"
#include "coreui_node.h"
#include "coreui_texture.h"
#include "cpp_hook.h"
#include "cpp_safe_ptr.h"
#include "imgui.h"
#include "imgui_internal.h"

namespace esc::coreui {
///
class Diagram {
 public:
  ///
  struct Hooks {
    ///
    cpp::Hook<auto()->bool> is_color_flow{};
    ///
    cpp::Hook<auto(float flow)->ImColor> get_flow_color{};
    ///
    cpp::Hook<void(Event event)> post_event{};
    ///
    cpp::Hook<auto(std::string_view file_path)->Texture> get_texture{};
  };

  ///
  Diagram(
      cpp::SafePtr<core::Diagram> diagram,
      cpp::SafePtr<const std::vector<std::unique_ptr<core::IFamily>>> families,
      cpp::SafePtr<core::IdGenerator> id_generator, Hooks hooks);

  ///
  void OnFrame();
  ///
  auto GetDiagram() const -> core::Diagram &;
  ///
  auto GetLinkCreation() -> LinkCreation &;
  ///
  auto GetFamilyGroups() -> const std::vector<FamilyGroup> &;
  ///
  auto GetNodes() const -> const std::vector<Node> &;
  ///
  auto GetLinks() const -> const std::vector<Link> &;

 private:
  ///
  auto FamilyFrom(const core::IFamily &core_family) const;
  ///
  void UpdateFamilyGroups();
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
  auto GetHeaderColor(const IHeaderTraits &header_traits,
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
  cpp::SafePtr<core::Diagram> diagram_;
  ///
  cpp::SafePtr<const std::vector<std::unique_ptr<core::IFamily>>> families_;
  ///
  cpp::SafePtr<core::IdGenerator> id_generator_;
  ///
  Hooks hooks_{};
  ///
  cpp::SafeOwner safe_owner_{};
  ///
  LinkCreation link_creation_;
  ///
  std::vector<FamilyGroup> family_groups_{};
  ///
  std::vector<Node> nodes_{};
  ///
  std::vector<Link> links_{};
};
}  // namespace esc::coreui

#endif  // VH_COREUI_DIAGRAM_H_
