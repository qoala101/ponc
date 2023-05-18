/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "coreui_linker.h"

#include <imgui.h>
#include <imgui_node_editor.h>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <optional>
#include <stack>
#include <vector>

#include "core_diagram.h"
#include "core_i_node.h"
#include "core_link.h"
#include "core_pin.h"
#include "coreui_diagram.h"
#include "coreui_event.h"
#include "coreui_flow_tree_node.h"
#include "cpp_assert.h"
#include "flow_algorithms.h"
#include "flow_tree_traversal.h"
#include "style_default_colors.h"
#include "style_default_sizes.h"

namespace vh::ponc::coreui {
namespace {
///
auto FindRootParent(ne::NodeId node_id,
                    const std::vector<flow::TreeNode>& flow_trees) -> auto& {
  const auto root_parent = std::find_if(
      flow_trees.cbegin(), flow_trees.cend(), [node_id](const auto& tree_node) {
        const auto found_node =
            flow::FindTreeNode(tree_node, [node_id](const auto& tree_node) {
              return tree_node.node_id == node_id;
            });

        return found_node.has_value();
      });

  Expects(root_parent != flow_trees.cend());
  return *root_parent;
}
}  // namespace

///
Linker::Linker(cpp::SafePtr<Diagram> parent_diagram)
    : parent_diagram_{std::move(parent_diagram)} {}

///
void Linker::SetPins(const std::optional<ne::PinId>& dragged_from_pin,
                     const std::optional<ne::PinId>& hovering_over_pin) {
  if (const auto creating_node = linking_data_.has_value() &&
                                 linking_data_->creating_data.has_value()) {
    return;
  }

  linking_data_.reset();

  if (!dragged_from_pin.has_value()) {
    return;
  }

  const auto& diagram = parent_diagram_->GetDiagram();
  const auto& dragged_from_node =
      core::Diagram::FindPinNode(diagram, *dragged_from_pin);

  linking_data_ = LinkingData{
      .dragged_from_pin_data = {
          .id = *dragged_from_pin,
          .kind = core::INode::GetPinKind(dragged_from_node, *dragged_from_pin),
          .node_id = dragged_from_node.GetId()}};

  const auto link_to_repin =
      core::Diagram::FindPinLink(diagram, *dragged_from_pin);

  if (link_to_repin.has_value()) {
    const auto fixed_pin =
        core::Link::GetOtherPin(**link_to_repin, *dragged_from_pin);

    linking_data_->repinning_data = RepinningData{
        .link_to_repin = (*link_to_repin)->id,
        .fixed_pin_data = {
            .id = fixed_pin,
            .kind = core::Link::GetPinKind(**link_to_repin, fixed_pin),
            .node_id = core::Diagram::FindPinNode(diagram, fixed_pin).GetId()}};
  }

  linking_data_->circular_pins_ = FindCircularPins();

  if (hovering_over_pin.has_value()) {
    const auto& hovering_over_pin_node =
        core::Diagram::FindPinNode(diagram, *hovering_over_pin);

    linking_data_->hovering_data = HoveringData{
        .hovering_over_pin = *hovering_over_pin,
        .hovering_over_pin_kind =
            core::INode::GetPinKind(hovering_over_pin_node, *hovering_over_pin),
        .reason = GetCanConnectToPinReason(*hovering_over_pin).second};
  }

  UpdateManualLinks();
}

///
auto Linker::CanConnectToPin(ne::PinId pin_id) const -> bool {
  return GetCanConnectToPinReason(pin_id).first;
}

///
auto Linker::GetSourcePinData() const -> auto& {
  Expects(linking_data_.has_value());

  if (const auto& repinning_data = linking_data_->repinning_data) {
    return repinning_data->fixed_pin_data;
  }

  return linking_data_->dragged_from_pin_data;
}

///
auto Linker::FindCircularPins() const
    -> std::unordered_set<core::IdValue<ne::PinId>> {
  const auto& source_pin = GetSourcePinData();
  const auto& flow_trees = parent_diagram_->GetFlowTrees();
  const auto& diagram = parent_diagram_->GetDiagram();

  if (source_pin.kind == ne::PinKind::Output) {
    const auto& root_parent = FindRootParent(source_pin.node_id, flow_trees);
    const auto& source_node =
        core::Diagram::FindNode(diagram, root_parent.node_id);

    if (const auto input_pin = source_node.GetInputPinId()) {
      return {input_pin->Get()};
    }

    return {};
  }

  Expects(linking_data_.has_value());
  const auto is_repinning = linking_data_->repinning_data.has_value();
  const auto& root_parent =
      is_repinning ? flow::FindTreeNode(flow_trees, source_pin.node_id)
                   : FindRootParent(source_pin.node_id, flow_trees);

  auto circular_pins = std::unordered_set<core::IdValue<ne::PinId>>{};

  flow::TraverseDepthFirst(
      root_parent,
      [&diagram, &circular_pins](const auto& tree_node) {
        const auto& node = core::Diagram::FindNode(diagram, tree_node.node_id);
        const auto& output_pins = node.GetOutputPinIds();

        for (const auto pin_id : output_pins) {
          if (!tree_node.child_nodes.contains(pin_id.Get())) {
            circular_pins.emplace(pin_id);
          }
        }
      },
      [](const auto&) {});

  return circular_pins;
}

///
auto Linker::CanConnectToFamily(const core::IFamily& family) const -> bool {
  if (!linking_data_.has_value()) {
    return true;
  }

  const auto sample_node = family.CreateSampleNode();
  const auto source_kind = GetSourcePinData().kind;
  const auto target_kind = core::Pin::GetOppositeKind(source_kind);

  return core::INode::FindFirstPinOfKind(*sample_node, target_kind).has_value();
}

///
auto Linker::GetCanCreateLinkReason() const -> std::pair<bool, std::string> {
  if (!linking_data_.has_value()) {
    return {true, {}};
  }

  const auto& hovering_data = linking_data_->hovering_data;

  if (!hovering_data.has_value()) {
    return {true, {}};
  }

  return GetCanConnectToPinReason(hovering_data->hovering_over_pin);
}

///
auto Linker::IsRepiningLink(ne::LinkId link_id) const -> bool {
  if (!linking_data_.has_value()) {
    return false;
  }

  const auto& repinning_data = linking_data_->repinning_data;

  if (!repinning_data.has_value()) {
    return false;
  }

  return repinning_data->link_to_repin == link_id;
}

///
auto Linker::GetManualLinks() const -> std::vector<ManualLink> {
  if (!linking_data_.has_value()) {
    return {};
  }

  return linking_data_->manual_links;
}

///
void Linker::AcceptCreateLink() {
  Expects(linking_data_.has_value());
  const auto& hovering_data = linking_data_->hovering_data;
  Expects(hovering_data.has_value());

  CreateLink(GetSourcePinData().id, hovering_data->hovering_over_pin);
  linking_data_.reset();
}

///
void Linker::StartCreatingNodeAt(const ImVec2& new_node_pos) {
  Expects(linking_data_.has_value());
  auto& creating_data = linking_data_->creating_data;
  Expects(!creating_data.has_value());

  creating_data = CreatingData{.new_node_pos = new_node_pos};
  UpdateManualLinks();

  const auto& source_pin = GetSourcePinData();

  if (source_pin.kind == ne::PinKind::Input) {
    return;
  }

  const auto& diagram = parent_diagram_->GetDiagram();
  const auto& source_node =
      core::Diagram::FindNode(diagram, source_pin.node_id);

  for (const auto pin_id : source_node.GetOutputPinIds()) {
    if (!core::Diagram::HasLink(diagram, pin_id)) {
      creating_data->source_node_empty_pins.emplace_back(pin_id);
    }
  }
}

///
auto Linker::CanCreateNodeForAllPins() const -> bool {
  if (!linking_data_.has_value()) {
    return false;
  }

  const auto& creating_data = linking_data_->creating_data;

  return creating_data.has_value() &&
         creating_data->source_node_empty_pins.size() > 1;
}

///
void Linker::SetCreateNodeForAllPins(bool create_for_all_pins) {
  Expects(linking_data_.has_value());
  auto& creating_data = linking_data_->creating_data;
  Expects(creating_data.has_value());

  creating_data->create_for_all_pins = create_for_all_pins;
  UpdateManualLinks();
}

///
void Linker::AcceptCreateNode(const Family& family) {
  Expects(linking_data_.has_value());
  const auto& creating_data = linking_data_->creating_data;
  Expects(creating_data.has_value());

  const auto& source_pin_data = GetSourcePinData();
  const auto target_kind = core::Pin::GetOppositeKind(source_pin_data.kind);
  const auto source_pins = creating_data->create_for_all_pins
                               ? creating_data->source_node_empty_pins
                               : std::vector{source_pin_data.id};

  auto created_nodes = std::vector<ne::NodeId>{};
  created_nodes.reserve(source_pins.size());

  auto first_created_node_pin = std::optional<ne::PinId>{};
  auto* last_link_created_event = static_cast<Event*>(nullptr);

  for (const auto source_pin : source_pins) {
    auto new_node = family.CreateNode();
    new_node->SetPos(creating_data->new_node_pos);
    created_nodes.emplace_back(new_node->GetId());

    const auto target_pin =
        core::INode::GetFirstPinOfKind(*new_node, target_kind);

    if (!first_created_node_pin.has_value()) {
      first_created_node_pin = target_pin;
    }

    parent_diagram_->AddNode(std::move(new_node));
    last_link_created_event = &CreateLink(source_pin, target_pin);
  }

  Expects(first_created_node_pin.has_value());

  auto& first_node_moved_event = last_link_created_event->Then(
      [parent_diagram = parent_diagram_,
       first_created_node_pin = *first_created_node_pin,
       new_node_pos = creating_data->new_node_pos]() {
        parent_diagram->GetNodeMover().MovePinTo(first_created_node_pin,
                                                 new_node_pos);
      });

  linking_data_.reset();

  if (created_nodes.size() <= 1) {
    return;
  }

  first_node_moved_event.Then([parent_diagram = parent_diagram_,
                               created_nodes = std::move(created_nodes)]() {
    const auto& first_node = core::Diagram::FindNode(
        parent_diagram->GetDiagram(), created_nodes.front());

    parent_diagram->GetNodeMover().ArrangeVerticallyAt(created_nodes,
                                                       first_node.GetPos());
  });
}

///
void Linker::DiscardCreateNode() { linking_data_.reset(); }

///
auto Linker::GetCanConnectToPinReason(ne::PinId pin_id) const
    -> std::pair<bool, std::string> {
  if (!linking_data_.has_value()) {
    return {true, {}};
  }

  if (linking_data_->circular_pins_.contains(pin_id.Get())) {
    return {false, "Same Tree"};
  }

  const auto& diagram = parent_diagram_->GetDiagram();
  const auto& repinning_data = linking_data_->repinning_data;
  const auto is_repinning = repinning_data.has_value();

  if (const auto pin_link = core::Diagram::FindPinLink(diagram, pin_id)) {
    if (const auto pin_of_repinning_link =
            is_repinning &&
            ((*pin_link)->id == repinning_data->link_to_repin)) {
      return {true, {}};
    }

    return {false, "Pin Is Taken"};
  }

  const auto& pin_node = core::Diagram::FindPinNode(diagram, pin_id);
  const auto& dragged_from_pin_data = linking_data_->dragged_from_pin_data;
  const auto source_node = is_repinning ? repinning_data->fixed_pin_data.node_id
                                        : dragged_from_pin_data.node_id;

  if (const auto same_node = pin_node.GetId() == source_node) {
    return {false, "Same Node"};
  }

  const auto pin_kind = core::INode::GetPinKind(pin_node, pin_id);
  const auto same_kind = pin_kind == dragged_from_pin_data.kind;

  if (const auto wrong_kind = is_repinning != same_kind) {
    return {false, "Wrong Pin Kind"};
  }

  if (is_repinning) {
    return {true, "Move Link"};
  }

  return {true, "Create Link"};
}

///
auto Linker::CreateManualLink(ne::PinId source_pin,
                              const PosVariant& target_pos,
                              const ImColor& color) const {
  auto manual_link = ManualLink{
      .color = color, .thickness = style::DefaultSizes::kBoldThickness};

  const auto source_kind = GetSourcePinData().kind;
  const auto source_pos =
      PosVariant{parent_diagram_->GetNodeMover().GetPinPos(source_pin)};

  if (source_kind == ne::PinKind::Input) {
    manual_link.start_pos = target_pos;
    manual_link.end_pos = source_pos;
  } else {
    manual_link.start_pos = source_pos;
    manual_link.end_pos = target_pos;
  }

  return manual_link;
}

///
auto Linker::GetRepinningLinkColor() const {
  Expects(linking_data_.has_value());

  const auto& hovering_data = linking_data_->hovering_data;

  if (!hovering_data.has_value() || (hovering_data->hovering_over_pin ==
                                     linking_data_->dragged_from_pin_data.id)) {
    return style::DefaultColors::kWhite;
  }

  if (const auto can_link_to_hovered_pin = GetCanCreateLinkReason().first) {
    return style::DefaultColors::kSuccess;
  }

  return style::DefaultColors::kError;
}

///
void Linker::UpdateManualLinks() {
  Expects(linking_data_.has_value());

  auto& manual_links = linking_data_->manual_links;
  manual_links.clear();

  const auto& creating_data = linking_data_->creating_data;

  if (const auto moving_link_around = !creating_data.has_value()) {
    if (const auto& repinning_data = linking_data_->repinning_data) {
      const auto repinning_link =
          CreateManualLink(repinning_data->fixed_pin_data.id, MousePos{},
                           GetRepinningLinkColor());

      manual_links.emplace_back(repinning_link);
    }

    return;
  }

  const auto& new_node_pos = creating_data->new_node_pos;
  const auto creating_link_color = style::DefaultColors::kWhite;

  if (!creating_data->create_for_all_pins) {
    const auto creating_link = CreateManualLink(
        GetSourcePinData().id, new_node_pos, creating_link_color);

    manual_links.emplace_back(creating_link);
    return;
  }

  for (const auto empty_source_pin : creating_data->source_node_empty_pins) {
    const auto creating_link =
        CreateManualLink(empty_source_pin, new_node_pos, creating_link_color);

    manual_links.emplace_back(creating_link);
  }
}

///
auto Linker::CreateLink(ne::PinId source_pin, ne::PinId target_pin) const
    -> Event& {
  Expects(linking_data_.has_value());

  const auto& source_pin_data = GetSourcePinData();

  if (const auto is_repinning_link =
          (source_pin == source_pin_data.id) &&
          linking_data_->repinning_data.has_value()) {
    return parent_diagram_->MoveLink(linking_data_->dragged_from_pin_data.id,
                                     target_pin);
  }

  if (source_pin_data.kind == ne::PinKind::Input) {
    return parent_diagram_->CreateLink(target_pin, source_pin);
  }

  return parent_diagram_->CreateLink(source_pin, target_pin);
}
}  // namespace vh::ponc::coreui