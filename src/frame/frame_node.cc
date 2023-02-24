#include "frame_node.h"

#include <variant>
#include <vector>

#include "coreui_i_node_drawer.h"
#include "cpp_assert.h"
#include "flow_tree.h"

namespace esc::frame {
namespace {
// ---
auto GetLinkAlpha [[nodiscard]] (const core::Link& link,
                                 const std::optional<ne::PinId>& fixed_pin) {
  if (fixed_pin.has_value() &&
      ((link.start_pin_id == *fixed_pin) || (link.end_pin_id == *fixed_pin))) {
    return 1.F / 2;
  }

  return 1.F;
}

auto GetLinks
    [[nodiscard]] (const core::Project& project, const draw::Widgets& widgets) {
  const auto& diagram = project.GetDiagram();
  const auto fixed_pin = widgets.new_link.FindFixedPin(diagram);

  const auto flow_tree = flow::BuildFlowTree(diagram);
  const auto node_flows = flow::CalculateNodeFlows(flow_tree);

  auto links = std::vector<Link>{};

  for (const auto& link : diagram.GetLinks()) {
    const auto alpha = GetLinkAlpha(link, fixed_pin);

    const auto start_pin_node = core::FindPinNode(diagram, link.start_pin_id);
    const auto node_flow = node_flows.at(start_pin_node->GetId().Get());
    const auto start_pin_flow = flow::GetPinFlow(node_flow, link.start_pin_id);
    auto color = widgets.settings_view.GetColorForFlowValue(
        start_pin_flow, project.GetSettings());
    color.Value.w = alpha;

    links.emplace_back(Link{.link = link, .color = color, .thickness = 2.F});
  }

  return links;
}

// ---
auto GetPinKind [[nodiscard]] (const coreui::IPinDrawer& pin_drawer,
                               const core::INode& node) {
  const auto pin_kind = pin_drawer.GetKind();

  if (pin_kind.has_value()) {
    return *pin_kind;
  }

  const auto pin_id = pin_drawer.GetPinId();
  Expects(pin_id.has_value());

  return GetPinKind(node, *pin_id);
}

// ---
auto GetPinIconAlpha
    [[nodiscard]] (ne::PinId pin_id, const core::Diagram& diagram,
                   const draw::NewLink& new_link) {
  auto alpha = 1.F;

  if (const auto creating_new_link = new_link.GetDraggedFromPin().has_value()) {
    if (!new_link.CanConnectToPin(pin_id, diagram)) {
      alpha /= 4;
    }
  }

  return alpha;
}

auto GetNodes
    [[nodiscard]] (const core::Project& project, const draw::Widgets& widgets) {
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

      if (widgets.settings_view.IsColorFlow()) {
        if (const auto input_flow = node_flow.input_pin_flow) {
          node_data.header->color = widgets.settings_view.GetColorForFlowValue(
              input_flow->second, settings);
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
      pin_data.color =
          widgets.settings_view.GetColorForFlowValue(pin_flow, settings);
      pin_data.color.Value.w =
          GetPinIconAlpha(*pin_data.id, diagram, widgets.new_link);
      pin_data.filled = FindPinLink(diagram, *pin_data.id).has_value();

      if (std::holds_alternative<std::monostate>(pin_data.value)) {
        pin_data.value = pin_flow;
      }
    }
  }

  return nodes;
}
}  // namespace

Frame::Frame(const core::Project& project, const draw::Widgets& widgets)
    : nodes{GetNodes(project, widgets)}, links{GetLinks(project, widgets)} {}
}  // namespace esc::frame