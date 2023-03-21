#ifndef VH_COREUI_DIAGRAM_H_
#define VH_COREUI_DIAGRAM_H_

#include <imgui_node_editor.h>

#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <unordered_set>
#include <variant>
#include <vector>

#include "core_diagram.h"
#include "core_i_family.h"
#include "core_i_node.h"
#include "core_link.h"
#include "coreui_event.h"
#include "coreui_family.h"
#include "coreui_flow_tree.h"
#include "coreui_i_header_traits.h"
#include "coreui_i_pin_traits.h"
#include "coreui_link.h"
#include "coreui_linker.h"
#include "coreui_node.h"
#include "coreui_node_mover.h"
#include "cpp_safe_ptr.h"
#include "flow_tree.h"

namespace esc::coreui {
///
class Project;

///
class Diagram {
 public:
  ///
  static auto FindNode(const Diagram &diagram, ne::NodeId node_id)
      -> const Node &;
  ///
  static auto FindNode(Diagram &diagram, ne::NodeId node_id) -> Node &;

  ///
  Diagram(cpp::SafePtr<Project> parent_project,
          cpp::SafePtr<core::Diagram> diagram);

  ///
  void OnFrame();
  ///
  auto GetDiagram() const -> core::Diagram &;
  ///
  auto GetNodeMover() const -> const NodeMover &;
  ///
  auto GetNodeMover() -> NodeMover &;
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
  auto AddNode(std::unique_ptr<core::INode> node) -> Event &;
  ///
  auto DeleteNode(ne::NodeId node_id) -> Event &;
  ///
  auto DeleteNodeWithLinks(ne::NodeId node_id) const -> Event &;
  ///
  auto CanReplaceNode(const core::INode &source_node,
                      const core::INode &target_node) -> bool;
  ///
  auto ReplaceNode(const core::INode &source_node,
                   std::unique_ptr<core::INode> target_node) -> Event &;
  ///
  auto GetLinks() const -> const std::vector<Link> &;
  ///
  auto CreateLink(ne::PinId start_pin_id, ne::PinId end_pin_id) const
      -> Event &;
  ///
  auto MoveLink(ne::PinId source_pin_id, ne::PinId target_pin_id) const
      -> Event &;
  ///
  auto DeleteLink(ne::LinkId link_id) const -> Event &;
  ///
  auto GetFlowTree() const -> const FlowTree &;

 private:
  ///
  auto GetFlowColor(float flow) const;
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
  auto NodeFlowFrom(const core::INode &core_node,
                    const flow::NodeFlow &core_flow) const;
  ///
  auto NodeFrom(core::INode &core_node, const flow::NodeFlow &node_flow) const;
  ///
  void UpdateNodes(const flow::NodeFlows &node_flows);
  ///
  auto GetFamilyNodes(core::FamilyId family_id) const;
  ///
  auto FamilyFrom(const core::IFamily &core_family) const;
  ///
  void UpdateFamilyGroups();
  ///
  void UpdateTreeNode(TreeNode &tree_node);
  ///
  void UpdateFlowTree(const flow::FlowTree &core_tree);
  ///
  auto GetFreePinFamily(ne::PinKind pin_kind) const -> auto &;
  ///
  auto IsFreePin(const core::INode &node) const;
  ///
  auto MoveConnectedLinkToNewFreePin(ne::PinId pin_id, ne::PinKind pin_kind,
                                     const coreui::Family &free_pin_family)
      -> Event &;

  ///
  cpp::SafePtr<Project> parent_project_;
  ///
  cpp::SafePtr<core::Diagram> diagram_;
  ///
  cpp::SafeOwner safe_owner_{};
  ///
  NodeMover node_mover_;
  ///
  Linker linker_;
  ///
  std::vector<FamilyGroup> family_groups_{};
  ///
  std::vector<Node> nodes_{};
  ///
  std::vector<Link> links_{};
  ///
  FlowTree flow_tree_{};
};
}  // namespace esc::coreui

#endif  // VH_COREUI_DIAGRAM_H_
