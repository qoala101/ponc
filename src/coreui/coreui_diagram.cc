
#include "coreui_diagram.h"

#include <algorithm>
#include <array>
#include <variant>

#include "core_diagram.h"
#include "core_i_node.h"
#include "coreui_i_node_traits.h"
#include "coreui_i_pin_traits.h"
#include "coreui_link.h"
#include "coreui_link_creation.h"
#include "coreui_pin.h"
#include "cpp_assert.h"
#include "flow_node_flow.h"
#include "flow_tree.h"
#include "imgui_node_editor.h"

namespace esc::coreui {
Diagram::Diagram(cpp::SafePointer<core::Diagram> diagram,
                 cpp::SafePointer<EventLoop> event_loop, Hooks hooks)
    : diagram_{std::move(diagram)},
      event_loop_{std::move(event_loop)},
      hooks_{std::move(hooks)},
      link_creation_{{.find_pin_node = [diagram = diagram_](ne::PinId pin_id)
                          -> const core::INode& {
                        return core::Diagram::FindPinNode(*diagram, pin_id);
                      },
                      .find_pin_link =
                          [diagram = diagram_](ne::PinId pin_id) {
                            return core::Diagram::FindPinLink(*diagram, pin_id);
                          }}} {}

void Diagram::OnFrame() {
  const auto flow_tree = flow::BuildFlowTree(*diagram_);
  const auto node_flows = flow::CalculateNodeFlows(flow_tree);
  const auto color_flow = hooks_.is_color_flow();

  UpdateNodes(node_flows, color_flow);
  UpdateLinks(node_flows, color_flow);
}

///
auto Diagram::GetPinIconAlpha(ne::PinId pin_id) const {
  if (link_creation_.IsCreatingLink() &&
      !link_creation_.CanConnectToPin(pin_id)) {
    return 1.F / 4;
  }

  return 1.F;
}

///
auto Diagram::GetFlowLinkAlpha(ne::LinkId link_id) const {
  if (link_creation_.IsCreatingLink() && link_creation_.IsRepinningLink() &&
      link_creation_.IsLinkBeingRepinned(link_id)) {
    return 1.F / 2;
  }

  return 1.F;
}

void Diagram::UpdateLinks(
    const std::unordered_map<uintptr_t, flow::NodeFlow>& node_flows,
    bool color_flow) {
  for (const auto& link : diagram_->GetLinks()) {
    auto& link_data = links_.emplace_back();
    link_data.type = link;
    link_data.thickness = 2.F;

    const auto link_alpha = GetFlowLinkAlpha(link.id);

    if (!color_flow) {
      link_data.color = {1.F, 1.F, 1.F, link_alpha};
      continue;
    }

    const auto& start_pin_node =
        core::Diagram::FindPinNode(*diagram_, link.start_pin_id).GetId().Get();
    Expects(node_flows.contains(start_pin_node));
    const auto& node_flow = node_flows.at(start_pin_node);
    const auto start_pin_flow = flow::GetPinFlow(node_flow, link.start_pin_id);

    link_data.color = hooks_.get_flow_color(start_pin_flow);
    link_data.color.Value.w = link_alpha;
  }

  if (!link_creation_.IsCreatingLink() || !link_creation_.IsRepinningLink()) {
    return;
  }

  auto& link_data = links_.emplace_back();
  link_data.thickness = 4.F;

  link_data.type = HandmadeLink{};

  const auto fixed_pin = link_creation_.GetFixedPinOfLinkBeingRepinned();
  const auto fixed_node_id =
      core::Diagram::FindPinNode(*diagram_, fixed_pin).GetId();
  const auto fixed_node = std::find_if(
      nodes_.begin(), nodes_.end(),
      [fixed_node_id](const auto& node) { return node.id == fixed_node_id; });

  Expects(fixed_node != nodes_.end());

  const auto& pin = Node::FindPin(*fixed_node, fixed_pin);
  Expects(pin.icon.has_value());

  pin.icon->GetRect();

  const auto fixed_pin_rect = nodes.pin_rects.at(fixed_pin.Get());
  const auto fixed_pin_node = FindPinNode(diagram, fixed_pin);
  const auto fixed_pin_kind = core::GetPinKind(*fixed_pin_node, fixed_pin);

  if (fixed_pin_kind == ax::NodeEditor::PinKind::Input) {
    curve.end_pos = ImVec2{fixed_pin_rect.Min.x,
                           (fixed_pin_rect.Min.y + fixed_pin_rect.Max.y) / 2};
  } else {
    curve.start_pos = ImVec2{fixed_pin_rect.Max.x,
                             (fixed_pin_rect.Min.y + fixed_pin_rect.Max.y)2};
  }

  if (link_creation_.IsHoveringOverPin()) {
    if (link_creation_.GetCanCreateLinkReason().first) {
      link_creation_.color = ImColor{1.F / 2, 1.F, 1.F / 2};
    } else {
      curve.color = ImColor{1.F, 1.F / 2, 1.F / 2};
    }
  }

  return curve;
}

void Diagram::UpdateNodes(
    const std::unordered_map<uintptr_t, flow::NodeFlow>& node_flows,
    bool color_flow) {
  nodes_.clear();

  // USE TRANSFORM

  for (const auto& node : diagram_->GetNodes()) {
    const auto node_id = node->GetId().Get();

    auto& node_data = nodes_.emplace_back();
    node_data.id = node_id;

    const auto node_traits = node->CreateUiTraits();

    Expects(node_flows.contains(node_id));
    const auto& node_flow = node_flows.at(node_id);

    if (node_traits->HasHeader()) {
      node_data.header = NodeHeader{.label = node_traits->GetLabel()};

      if (color_flow) {
        if (const auto input_flow = node_flow.input_pin_flow) {
          node_data.header->color = hooks_.get_flow_color(input_flow->second);
        } else {
          node_data.header->color = {1.F, 1.F, 1.F};
        }
      } else {
        node_data.header->color = node_traits->GetColor();
      }
    }

    // USE TRANSFORM
    for (const auto& pin_traits : node_traits->CreatePinTraits()) {
      auto& pin_data =
          (IPinTraits::GetPinKind(*pin_traits, *node) == ne::PinKind::Input)
              ? node_data.input_pins.emplace_back()
              : node_data.output_pins.emplace_back();
      pin_data.value = pin_traits->GetValue();
      pin_data.label = pin_traits->GetLabel();

      const auto pin = pin_traits->GetPin();

      if (!std::holds_alternative<ne::PinId>(pin)) {
        continue;
      }

      const auto pin_id = std::get<ne::PinId>(pin);
      const auto pin_flow = flow::GetPinFlow(node_flow, pin_id);

      pin_data.icon = PinIcon{
          {.id = pin_id,
           .color = hooks_.get_flow_color(pin_flow),
           .filled =
               core::Diagram::FindPinLink(*diagram_, pin_id).has_value()}};

      if (std::holds_alternative<std::monostate>(pin_data.value)) {
        pin_data.value = pin_flow;
      }
    }
  }
}

// void Diagram::EmplaceNode(std::shared_ptr<core::INode> node,
//                           const ImVec2& position) {
//   events_.emplace_back([this, node = std::move(node), position]() mutable {
//     const auto& new_node =
//         signal_get_project_().GetDiagram().EmplaceNode(std::move(node));
//     new_node->SetPosition(position);
//   });
// }

// void Diagram::CreateLink(ne::PinId start_pin_id, ne::PinId end_pin_id) {
//   events_.emplace_back([this, start_pin_id, end_pin_id]() {
//     auto& project = signal_get_project_();

//     project.GetDiagram().EmplaceLink(
//         {.id = project.GetIdGenerator().Generate<ne::LinkId>(),
//          .start_pin_id = start_pin_id,
//          .end_pin_id = end_pin_id});
//   });
// }

// void Project::DeleteLink(ne::LinkId link_id) {
//   events_.emplace_back([this, link_id]() {
//     ne::DeleteLink(link_id);
//     signal_get_project_().GetDiagram().EraseLink(link_id);
//   });
// }
}  // namespace esc::coreui