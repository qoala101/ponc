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
#include "core_link.h"
#include "coreui_family.h"
#include "coreui_i_header_traits.h"
#include "coreui_i_pin_traits.h"
#include "coreui_link.h"
#include "coreui_linker.h"
#include "coreui_node.h"
#include "cpp_safe_ptr.h"

namespace esc::coreui {
///
class Project;

///
class Diagram {
 public:
  ///
  Diagram(cpp::SafePtr<Project> parent_project,
          cpp::SafePtr<core::Diagram> diagram);

  ///
  void OnFrame();
  ///
  auto GetDiagram() const -> core::Diagram &;
  ///
  auto GetLinker() const -> const Linker &;
  ///
  auto GetLinker() -> Linker &;
  ///
  auto GetFamilyGroups() const -> const std::vector<FamilyGroup> &;
  ///
  auto GetNodes() const -> const std::vector<Node> &;
  ///
  auto GetNodes() -> std::vector<Node> &;
  ///
  void AddNode(std::unique_ptr<core::INode> node) const;
  void DeleteNode(ne::NodeId node_id) const;
  void DeleteNodeWithLinks(ne::NodeId node_id) const;
  ///
  auto GetLinks() const -> const std::vector<Link> &;
  ///
  void CreateLink(ne::PinId start_pin_id, ne::PinId end_pin_id) const;
  ///
  void DeleteLink(ne::LinkId link_id) const;

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
  auto NodeFrom(core::INode &core_node, const flow::NodeFlow &node_flow) const;
  ///
  void UpdateNodes(const flow::NodeFlows &node_flows);
  void MoveConnectedLinkToNewFreePin(
      ne::PinId pin_id, ne::PinKind pin_kind,
      const core::IFamily &free_pin_family) const;

  ///
  cpp::SafePtr<Project> parent_project_;
  ///
  cpp::SafePtr<core::Diagram> diagram_;
  ///
  cpp::SafeOwner safe_owner_{};
  ///
  Linker linker_;
  ///
  std::vector<FamilyGroup> family_groups_{};
  ///
  std::vector<Node> nodes_{};
  ///
  std::vector<Link> links_{};
};
}  // namespace esc::coreui

#endif  // VH_COREUI_DIAGRAM_H_
