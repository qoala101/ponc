/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_COREUI_DIAGRAM_H_
#define VH_PONC_COREUI_DIAGRAM_H_

#include <imgui.h>
#include <imgui_node_editor.h>

#include <memory>
#include <vector>

#include "core_area.h"
#include "core_diagram.h"
#include "core_i_family.h"
#include "core_i_node.h"
#include "core_link.h"
#include "coreui_area_creator.h"
#include "coreui_event.h"
#include "coreui_family.h"
#include "coreui_flow_tree_node.h"
#include "coreui_i_header_traits.h"
#include "coreui_i_pin_traits.h"
#include "coreui_link.h"
#include "coreui_linker.h"
#include "coreui_node.h"
#include "coreui_node_mover.h"
#include "coreui_node_replacer.h"
#include "coreui_pin.h"
#include "cpp_safe_ptr.h"
#include "flow_node_flow.h"
#include "flow_tree_node.h"

namespace vh::ponc::coreui {
///
class Diagram {
 public:
  ///
  Diagram(cpp::SafePtr<Project> parent_project,
          cpp::SafePtr<core::Diagram> diagram);

  ///
  static auto FindNode(const Diagram &diagram, ne::NodeId node_id)
      -> const Node &;
  ///
  static auto FindNode(Diagram &diagram, ne::NodeId node_id) -> Node &;
  ///
  static auto FindLink(const Diagram &diagram, ne::LinkId link_id)
      -> const Link &;

  ///
  void OnFrame();
  ///
  auto GetDiagram() const -> core::Diagram &;
  ///
  auto GetFlowTrees() const -> const std::vector<flow::TreeNode> &;
  ///
  auto GetNodeMover() const -> const NodeMover &;
  ///
  auto GetNodeMover() -> NodeMover &;
  ///
  auto GetNodeReplacer() const -> const NodeReplacer &;
  ///
  auto GetLinker() const -> const Linker &;
  ///
  auto GetLinker() -> Linker &;
  ///
  auto GetAreaCreator() -> AreaCreator &;
  ///
  auto GetFamilyGroups() const -> const std::vector<FamilyGroup> &;
  ///
  auto GetNodes() const -> const std::vector<Node> &;
  ///
  auto GetNodes() -> std::vector<Node> &;
  ///
  auto AddNode(std::unique_ptr<core::INode> node) const -> Event &;
  ///
  auto DeleteNode(ne::NodeId node_id) -> Event &;
  ///
  auto DeleteNodeWithLinks(ne::NodeId node_id) const -> Event &;
  ///
  void TreeSelect(const std::vector<ne::NodeId> &node_ids);
  ///
  void TreeArrange(const std::vector<ne::NodeId> &node_ids);
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
  auto GetNodeTrees() const -> const std::vector<TreeNode> &;
  ///
  auto AddArea(const core::Area &area) -> Event &;
  ///
  auto DeleteArea(core::AreaId area_id) -> Event &;

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
  void UpdateNodeTrees();
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
  std::vector<flow::TreeNode> flow_trees_{};
  ///
  NodeMover node_mover_;
  ///
  NodeReplacer node_replacer_;
  ///
  Linker linker_;
  ///
  AreaCreator area_creator_;
  ///
  std::vector<FamilyGroup> family_groups_{};
  ///
  std::vector<Node> nodes_{};
  ///
  std::vector<Link> links_{};
  ///
  std::vector<TreeNode> node_trees_{};
};
}  // namespace vh::ponc::coreui

#endif  // VH_PONC_COREUI_DIAGRAM_H_
