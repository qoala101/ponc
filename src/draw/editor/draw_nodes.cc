/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#include "draw_nodes.h"

#include <iostream>
#include <optional>
#include <string_view>

#include "app_state.h"
#include "core_diagram.h"
#include "core_i_node.h"
#include "core_project.h"
#include "core_settings.h"
#include "coreui_i_node_drawer.h"
#include "coreui_i_pin_drawer.h"
#include "cpp_assert.h"
#include "cpp_scope.h"
#include "draw_flow_icon.h"
#include "draw_new_link.h"
#include "draw_node_drawer.h"
#include "draw_widgets.h"
#include "flow_node_flow.h"
#include "flow_tree.h"
#include "imgui.h"
#include "imgui_node_editor.h"

namespace esc::draw {
namespace {
// ---
void DrawHeader(const coreui::INodeDrawer& node_drawer) {
  ImGui::Spring(0);
  ImGui::TextUnformatted(node_drawer.GetLabel().c_str());
  ImGui::Spring(1);
  ImGui::Dummy(ImVec2{0, 28});
  ImGui::Spring(0);
}

// ---
void DrawEmptyPinArea(ImVec2 area_size, bool pin_editable) {
  if (pin_editable) {
    area_size.x -= 4;
  }

  ImGui::Dummy(area_size);
}

// ---
auto GetPinIconAlpha [[nodiscard]] (const std::optional<NewLink>& new_link) {
  auto alpha = 255;

  // if (new_link.has_value()) {
  //   if (!new_link
  //            ->CanConnectToPin(app_state.project.GetDiagram(),
  //                              *pin_drawer.GetPinId())
  //            .valid) {
  //     alpha /= 2;
  //   }
  // }

  return alpha;
}

// ---
auto GetPinIconColor [[nodiscard]] (const core::Settings& settings,
                                    const SettingsView& settings_view) {
  // settings_view.
  const auto color = ImColor{255, 255, 255};
}

// ---
void DrawPinField(coreui::IPinDrawer& pin_drawer,
                  const flow::NodeFlow& node_flow) {
  const auto spring_scope =
      cpp::Scope{[]() { ImGui::Spring(0); }, []() { ImGui::Spring(0); }};

  const auto label = pin_drawer.GetLabel();

  if (const auto float_value = pin_drawer.GetFloat()) {
    if (pin_drawer.IsEditable()) {
      ImGui::SetNextItemWidth(100);
      ImGui::InputFloat(label.c_str(), *float_value, 0.F, 0.F, "%.3f");
      return;
    }

    ImGui::Text("%.3f %s", **float_value, label.c_str());
    return;
  }

  if (const auto pin_id = pin_drawer.GetPinId()) {
    const auto pin_flow = GetPinFlow(node_flow, *pin_id);
    ImGui::Text("%.3f %s", pin_flow, label.c_str());
    return;
  }

  ImGui::TextUnformatted(label.c_str());
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
                   const NewLink& new_link) {
  auto alpha = 1.F;

  if (const auto creating_new_link = new_link.GetDraggedFromPin().has_value()) {
    if (!new_link.CanConnectToPin(pin_id, diagram)) {
      alpha /= 2;
    }
  }

  return alpha;
}

void DrawFlowIconPinArea(ne::PinId pin_id, ImVec2 area_size,
                         const core::Diagram& diagram, const NewLink& new_link,
                         const SettingsView& settings_view) {
  if (!ImGui::IsRectVisible(area_size)) {
    return;
  }

  const auto alpha = GetPinIconAlpha(pin_id, diagram, new_link);
  const auto color = ImColor{1.F, 1.F, 1.F, alpha};
  const auto filled = FindPinLink(diagram, pin_id).has_value();
  DrawFlowIcon(area_size, color, filled);
  ImGui::Dummy(area_size);
}
}  // namespace

// ---
Nodes::Nodes(const Texture& node_header_texture)
    : node_header_texture_{node_header_texture} {}

void Nodes::Draw(const AppState& app_state) {
  drawn_pin_icon_rects_.clear();

  const auto& diagram = app_state.project.GetDiagram();
  const auto& nodes = diagram.GetNodes();
  const auto flow_tree = flow::BuildFlowTree(diagram);
  const auto node_flows = flow::CalculateNodeFlows(flow_tree);
  const auto& new_link = app_state.widgets.new_link;

  for (const auto& node : nodes) {
    const auto& node_flow = node_flows.at(node->GetId().Get());
    DrawNode(*node, node_flow, diagram, new_link,
             app_state.widgets.settings_view);
  }
}

// ---
auto Nodes::GetDrawnPinIconRect(ne::PinId pin_id) const -> const ImRect& {
  return drawn_pin_icon_rects_.at(pin_id.Get());
}

// ---
void Nodes::DrawNode(core::INode& node, const flow::NodeFlow& node_flow,
                     const core::Diagram& diagram, const NewLink& new_link,
                     const SettingsView& settings_view) {
  const auto node_drawer = node.CreateDrawer();
  auto node_builder = esc::NodeDrawer{node.GetId()};

  if (node_drawer->HasHeader()) {
    const auto header_scope =
        node_builder.AddHeader(node_header_texture_, node_drawer->GetColor());

    DrawHeader(*node_drawer);
  }

  for (const auto& pin_drawer : node_drawer->CreatePinDrawers()) {
    const auto pin_kind = GetPinKind(*pin_drawer, node);

    {
      const auto pin_scope =
          node_builder.AddPin(pin_drawer->GetPinId(), pin_kind);

      if (pin_kind == ne::PinKind::Input) {
        DrawPinIconArea(*pin_drawer, diagram, new_link, settings_view);
        DrawPinField(*pin_drawer, node_flow);
      } else {
        DrawPinField(*pin_drawer, node_flow);
        DrawPinIconArea(*pin_drawer, diagram, new_link, settings_view);
      }
    }
  }
}

// ---
void Nodes::DrawPinIconArea(const coreui::IPinDrawer& pin_drawer,
                            const core::Diagram& diagram,
                            const NewLink& new_link,
                            const SettingsView& settings_view) {
  const auto pin_id = pin_drawer.GetPinId();
  const auto area_size = ImVec2{24, 24};

  if (!pin_id.has_value()) {
    DrawEmptyPinArea(area_size, pin_drawer.IsEditable());
    return;
  }

  DrawFlowIconPinArea(*pin_id, area_size, diagram, new_link, settings_view);

  drawn_pin_icon_rects_[pin_id->Get()] =
      ImRect{ImGui::GetItemRectMin(), ImGui::GetItemRectMax()};
}
}  // namespace esc::draw