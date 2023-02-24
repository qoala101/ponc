#include "frame_node.h"

#include <variant>
#include <vector>

#include "coreui_i_node_drawer.h"
#include "cpp_assert.h"
#include "draw_node_editor.h"
#include "flow_tree.h"
#include "imgui.h"

namespace esc::frame {
namespace {
auto GetLinkAlpha(const core::Link& link,
                  const std::optional<ne::PinId>& fixed_pin) {
  if (fixed_pin.has_value() &&
      ((link.start_pin_id == *fixed_pin) || (link.end_pin_id == *fixed_pin))) {
    return 1.F / 2;
  }

  return 1.F;
}

auto GetColorForFlowValue(float value, const core::Settings& settings)
    -> ImColor {
  if (!settings.color_flow) {
    return ImColor{1.F, 1.F, 1.F};
  }

  const auto blue = ImColor{0.F, 0.F, 1.F};
  const auto red = ImColor{1.F, 0.F, 0.F};

  if (value < settings.min) {
    return blue;
  }

  if (value >= settings.max) {
    return red;
  }

  const auto range = (settings.max - settings.min);
  const auto value_percentage = (value - settings.min) / range;
  const auto low_percentage = (settings.low - settings.min) / range;
  const auto high_percentage = (settings.high - settings.min) / range;

  auto percentage = 0.0F;
  auto start_color = ImColor{};
  auto end_color = ImColor{};

  const auto blue_green = ImColor{0.F, 1.F, 1.F};
  const auto green = ImColor{0.F, 1.F, 0.F};
  const auto green_red = ImColor{1.F, 1.F, 0.F};

  if (value_percentage < low_percentage) {
    percentage = value_percentage / low_percentage;
    start_color = blue;
    end_color = blue_green;
  } else if (value_percentage >= high_percentage) {
    percentage =
        (value_percentage - high_percentage) / (1.0F - high_percentage);
    start_color = green_red;
    end_color = red;
  } else {
    const auto low_high_range = (high_percentage - low_percentage);
    percentage = (value_percentage - low_percentage) / low_high_range;

    if (percentage < 0.5F) {
      percentage = percentage * 2;
      start_color = blue_green;
      end_color = green;
    } else {
      percentage = (percentage - 0.5F) * 2;
      start_color = green;
      end_color = green_red;
    }
  }

  return ImColor{start_color.Value.x +
                     percentage * (end_color.Value.x - start_color.Value.x),
                 start_color.Value.y +
                     percentage * (end_color.Value.y - start_color.Value.y),
                 start_color.Value.z +
                     percentage * (end_color.Value.z - start_color.Value.z)};
}

auto FindFixedPin(const core::Project& project, const draw::Widgets& widgets)
    -> std::optional<ne::PinId> {
  const auto& new_link = widgets.node_editor.GetNewLink();
  const auto dragged_from_pin_ = new_link.GetDraggedFromPin();

  if (!dragged_from_pin_.has_value()) {
    return std::nullopt;
  }

  const auto link_to_repin =
      FindPinLink(project.GetDiagram(), *dragged_from_pin_);

  if (!link_to_repin.has_value()) {
    return std::nullopt;
  }

  if (*dragged_from_pin_ == (*link_to_repin)->start_pin_id) {
    return (*link_to_repin)->end_pin_id;
  }

  return (*link_to_repin)->start_pin_id;
}

auto GetLinks(const core::Project& project, const draw::Widgets& widgets) {
  const auto& diagram = project.GetDiagram();
  const auto fixed_pin = FindFixedPin(project, widgets);

  const auto flow_tree = flow::BuildFlowTree(diagram);
  const auto node_flows = flow::CalculateNodeFlows(flow_tree);

  auto links = std::vector<Link>{};

  for (const auto& link : diagram.GetLinks()) {
    const auto alpha = GetLinkAlpha(link, fixed_pin);

    const auto start_pin_node = core::FindPinNode(diagram, link.start_pin_id);
    const auto node_flow = node_flows.at(start_pin_node->GetId().Get());
    const auto start_pin_flow = flow::GetPinFlow(node_flow, link.start_pin_id);
    auto color = GetColorForFlowValue(start_pin_flow, project.GetSettings());
    color.Value.w = alpha;

    auto& link_data = links.emplace_back();
    link_data.id = link.id;
    link_data.start_pin_id = link.start_pin_id;
    link_data.end_pin_id = link.end_pin_id;
    link_data.color = color;
    link_data.thickness = 2.F;
  }

  return links;
}

auto GetPinKind(const coreui::IPinDrawer& pin_drawer, const core::INode& node) {
  const auto pin_kind = pin_drawer.GetKind();

  if (pin_kind.has_value()) {
    return *pin_kind;
  }

  const auto pin_id = pin_drawer.GetPinId();
  Expects(pin_id.has_value());

  return GetPinKind(node, *pin_id);
}

auto GetCanConnectToPinReason(ne::PinId pin_id, const core::Project& project,
                              const draw::Widgets& widgets)
    -> std::pair<bool, std::string> {
  const auto& new_link = widgets.node_editor.GetNewLink();
  const auto& diagram = project.GetDiagram();
  const auto dragged_from_pin_ = new_link.GetDraggedFromPin();

  Expects(dragged_from_pin_.has_value());

  const auto rebinding_link = core::FindPinLink(diagram, *dragged_from_pin_);
  const auto hovered_over_link = core::FindPinLink(diagram, pin_id);

  if (hovered_over_link.has_value()) {
    if (rebinding_link.has_value() &&
        ((*hovered_over_link)->id == (*rebinding_link)->id)) {
      return {true, {}};
    }

    return {false, "Pin Is Occupied"};
  }

  const auto& dragged_from_node = FindPinNode(diagram, *dragged_from_pin_);
  const auto& hovered_over_node = FindPinNode(diagram, pin_id);

  if (dragged_from_node->GetId() == hovered_over_node->GetId()) {
    return {false, "Self"};
  }

  if (const auto fixed_pin = FindFixedPin(project, widgets)) {
    const auto fixed_pin_node = FindPinNode(diagram, *fixed_pin);

    if (fixed_pin_node->GetId() == hovered_over_node->GetId()) {
      return {false, "Self"};
    }
  }

  const auto dragged_from_kind =
      GetPinKind(*dragged_from_node, *dragged_from_pin_);
  const auto hovered_over_kind = GetPinKind(*hovered_over_node, pin_id);

  if (rebinding_link.has_value()) {
    if (dragged_from_kind != hovered_over_kind) {
      return {false, "Wrong Pin Kind"};
    }
  } else {
    if (dragged_from_kind == hovered_over_kind) {
      return {false, "Wrong Pin Kind"};
    }
  }

  if (rebinding_link.has_value()) {
    return {true, "Move Link"};
  }

  return {true, "Create Link"};
}

auto CanConnectToPin(ne::PinId pin_id, const core::Project& project,
                     const draw::Widgets& widgets) -> bool {
  return GetCanConnectToPinReason(pin_id, project, widgets).first;
}

auto GetPinIconAlpha(ne::PinId pin_id, const core::Project& project,
                     const draw::Widgets& widgets) {
  const auto& new_link = widgets.node_editor.GetNewLink();
  auto alpha = 1.F;

  if (const auto creating_new_link = new_link.GetDraggedFromPin().has_value()) {
    if (!CanConnectToPin(pin_id, project, widgets)) {
      alpha /= 4;
    }
  }

  return alpha;
}

auto GetNodes(const core::Project& project, const draw::Widgets& widgets) {
  auto nodes = std::vector<Node>{};
  const auto& diagram = project.GetDiagram();
  const auto flow_tree = flow::BuildFlowTree(project.GetDiagram());
  const auto node_flows = flow::CalculateNodeFlows(flow_tree);
  const auto& settings = project.GetSettings();

  for (const auto& node : project.GetDiagram().GetNodes()) {
    auto& node_data = nodes.emplace_back();
    node_data.id = node->GetId();

    const auto drawer = node->CreateDrawer();
    const auto& node_flow = node_flows.at(node_data.id.Get());

    if (drawer->HasHeader()) {
      node_data.header =
          NodeHeader{.color = drawer->GetColor(), .label = drawer->GetLabel()};

      if (project.GetSettings().color_flow) {
        if (const auto input_flow = node_flow.input_pin_flow) {
          node_data.header->color =
              GetColorForFlowValue(input_flow->second, settings);
        } else {
          node_data.header->color = ImColor{1.F, 1.F, 1.F};
        }
      }
    }

    for (const auto& pin_drawer : drawer->CreatePinDrawers()) {
      const auto pin_kind = GetPinKind(*pin_drawer, *node);
      auto& pin_data = (pin_kind == ne::PinKind::Input)
                           ? node_data.input_pins.emplace_back()
                           : node_data.output_pins.emplace_back();

      pin_data.id = pin_drawer->GetPinId();
      pin_data.label = pin_drawer->GetLabel();

      if (const auto float_value = pin_drawer->GetFloat()) {
        pin_data.value = *float_value;
      }

      if (!pin_data.id.has_value()) {
        continue;
      }

      const auto pin_flow = flow::GetPinFlow(node_flow, *pin_data.id);
      pin_data.color = GetColorForFlowValue(pin_flow, settings);
      pin_data.color.Value.w = GetPinIconAlpha(*pin_data.id, project, widgets);
      pin_data.filled = FindPinLink(diagram, *pin_data.id).has_value();

      if (std::holds_alternative<std::monostate>(pin_data.value)) {
        pin_data.value = pin_flow;
      }
    }
  }

  return nodes;
}

auto GetCurve(const core::Project& project, const draw::Widgets& widgets)
    -> std::optional<Curve> {
  const auto& new_link = widgets.node_editor.GetNewLink();
  const auto& diagram = project.GetDiagram();
  const auto fixed_pin = FindFixedPin(project, widgets);

  if (!fixed_pin.has_value()) {
    return std::nullopt;
  }

  auto curve = Curve{.color = ImColor{1.F, 1.F, 1.F}, .thickness = 4.F};

  const auto fixed_pin_rect =
      widgets.node_editor.GetNodes().GetDrawnPinIconRect(*fixed_pin);
  const auto fixed_pin_node = FindPinNode(diagram, *fixed_pin);
  const auto fixed_pin_kind = core::GetPinKind(*fixed_pin_node, *fixed_pin);

  if (fixed_pin_kind == ax::NodeEditor::PinKind::Input) {
    curve.end_position =
        ImVec2{fixed_pin_rect.Min.x,
               (fixed_pin_rect.Min.y + fixed_pin_rect.Max.y) / 2};
  } else {
    curve.start_position =
        ImVec2{fixed_pin_rect.Max.x,
               (fixed_pin_rect.Min.y + fixed_pin_rect.Max.y) / 2};
  }

  if (const auto hovering_over_pin = new_link.GetHoveringOverPin()) {
    if (CanConnectToPin(*hovering_over_pin, project, widgets)) {
      curve.color = ImColor{1.F / 2, 1.F, 1.F / 2};
    } else {
      curve.color = ImColor{1.F, 1.F / 2, 1.F / 2};
    }
  }

  return curve;
}

auto GetAlphaForNewLink(const core::Project& project,
                        const draw::Widgets& widgets) -> float {
  if (FindFixedPin(project, widgets).has_value()) {
    return 0.F;
  }

  return 1.F;
}

auto GetCreation(const core::Project& project, const draw::Widgets& widgets)
    -> std::optional<Creation> {
  const auto& new_link = widgets.node_editor.GetNewLink();

  if (!new_link.GetDraggedFromPin().has_value()) {
    return std::nullopt;
  }

  const auto hovering_over_pin_ = new_link.GetHoveringOverPin();

  if (!hovering_over_pin_.has_value()) {
    return std::nullopt;
  }
  const auto& [can_connect, reason] =
      GetCanConnectToPinReason(*hovering_over_pin_, project, widgets);

  auto creation = Creation{.can_connect = can_connect, .reason = reason};

  const auto& diagram = project.GetDiagram();
  const auto hovering_over_node = FindPinNode(diagram, *hovering_over_pin_);
  const auto hovering_over_pin_kind =
      core::GetPinKind(*hovering_over_node, *hovering_over_pin_);

  if (const auto fixed_pin = FindFixedPin(project, widgets)) {
    const auto link_to_repin = core::FindPinLink(diagram, *fixed_pin);
    Expects(link_to_repin.has_value());

    creation.delete_link = (*link_to_repin)->id;

    if (hovering_over_pin_kind == ne::PinKind::Input) {
      creation.start_pin_id = *fixed_pin;
      creation.end_pin_id = *hovering_over_pin_;
    } else {
      creation.start_pin_id = *hovering_over_pin_;
      creation.end_pin_id = *fixed_pin;
    }
  } else {
    const auto dragged_from_pin_ = new_link.GetDraggedFromPin();
    Expects(dragged_from_pin_.has_value());  // why?

    if (hovering_over_pin_kind == ne::PinKind::Input) {
      creation.start_pin_id = *dragged_from_pin_;
      creation.end_pin_id = *hovering_over_pin_;
    } else {
      creation.start_pin_id = *hovering_over_pin_;
      creation.end_pin_id = *dragged_from_pin_;
    }
  }

  return creation;
}
}  // namespace

Frame::Frame(const core::Project& project, const draw::Widgets& widgets)
    : nodes{GetNodes(project, widgets)},
      links{GetLinks(project, widgets)},
      curve{GetCurve(project, widgets)},
      creation_alpha{GetAlphaForNewLink(project, widgets)},
      creation{GetCreation(project, widgets)} {}
}  // namespace esc::frame