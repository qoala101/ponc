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
#include "coreui_event_loop.h"
#include "coreui_family.h"
#include "coreui_i_header_traits.h"
#include "coreui_i_pin_traits.h"
#include "coreui_link.h"
#include "coreui_linking.h"
#include "coreui_node.h"
#include "coreui_texture.h"
#include "cpp_callbacks.h"
#include "cpp_safe_ptr.h"
#include "imgui.h"

namespace esc::coreui {
///
class Diagram {
 public:
  ///
  struct Callbacks {
    ///
    cpp::Query<bool> is_color_flow{};
    ///
    cpp::Query<ImColor, float> get_flow_color{};
    ///
    cpp::Query<Texture, std::string_view> get_texture{};
    ///
    cpp::Action<void(Event)> post_event{};
  };

  ///
  Diagram(
      cpp::SafePtr<core::Diagram> diagram,
      cpp::SafePtr<const std::vector<std::unique_ptr<core::IFamily>>> families,
      cpp::SafePtr<core::IdGenerator> id_generator, Callbacks callbacks);

  ///
  void OnFrame();
  ///
  auto GetDiagram() const -> core::Diagram &;
  ///
  auto GetLinking() const -> const Linking &;
  ///
  auto GetLinking() -> Linking &;
  ///
  auto GetFamilyGroups() -> const std::vector<FamilyGroup> &;
  ///
  auto GetNodes() const -> const std::vector<Node> &;
  ///
  auto GetNodes() -> std::vector<Node> &;
  ///
  void EmplaceNode(std::unique_ptr<core::INode> node);
  ///
  auto GetLinks() const -> const std::vector<Link> &;

 private:
  ///
  auto FamilyFrom(const core::IFamily &core_family) const;
  ///
  void UpdateFamilyGroups();
  ///
  auto LinkFrom(const core::Link &core_link,
                const flow::NodeFlows &node_flows) const;
  ///
  void UpdateLinks(const flow::NodeFlows &node_flows);
  ///
  auto GetHeaderColor(const IHeaderTraits &header_traits,
                      const flow::NodeFlow &node_flow) const;
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
  Callbacks callbacks_{};
  ///
  cpp::SafeOwner safe_owner_{};
  ///
  Linking linking_;
  ///
  std::vector<FamilyGroup> family_groups_{};
  ///
  std::vector<Node> nodes_{};
  ///
  std::vector<Link> links_{};
};
}  // namespace esc::coreui

#endif  // VH_COREUI_DIAGRAM_H_
