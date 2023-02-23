/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#include "flow_node_flow.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>

#include "app_events.h"
#include "app_state.h"
#include "core_diagram.h"
#include "core_i_node.h"
#include "core_project.h"
#include "cpp_assert.h"
#include "cpp_scope.h"
#include "draw_links.h"
#include "draw_tooltip.h"
#include "draw_widgets.h"
#include "flow_tree.h"
#include "imgui_bezier_math.h"
#include "imgui_node_editor.h"

namespace esc::draw {
namespace {
auto GetCurve(const ImVec2& m_Start, const ImVec2& m_End, ne::PinKind kind) {
  const auto strength = ne::GetStyle().LinkStrength;
  const auto start_dir = kind == ne::PinKind::Output
                             ? ne::GetStyle().SourceDirection
                             : ne::GetStyle().TargetDirection;
  const auto end_dir = kind == ne::PinKind::Input
                           ? ne::GetStyle().SourceDirection
                           : ne::GetStyle().TargetDirection;

  auto easeLinkStrength = [](const ImVec2& a, const ImVec2& b, float strength) {
    const auto distanceX = b.x - a.x;
    const auto distanceY = b.y - a.y;
    const auto distance = ImSqrt(distanceX * distanceX + distanceY * distanceY);
    const auto halfDistance = distance * 0.5f;

    if (halfDistance < strength)
      strength = strength * ImSin(IM_PI * 0.5f * halfDistance / strength);

    return strength;
  };

  const auto startStrength = easeLinkStrength(m_Start, m_End, strength);
  const auto endStrength = easeLinkStrength(m_Start, m_End, strength);
  const auto cp0 = m_Start + start_dir * startStrength;
  const auto cp1 = m_End + end_dir * endStrength;

  ImCubicBezierPoints result;
  result.P0 = m_Start;
  result.P1 = cp0;
  result.P2 = cp1;
  result.P3 = m_End;

  return result;
}

// ---
auto GetLinkAlpha [[nodiscard]] (const core::Link& link,
                                 const std::optional<ne::PinId>& fixed_pin) {
  if (fixed_pin.has_value() &&
      ((link.start_pin_id == *fixed_pin) || (link.end_pin_id == *fixed_pin))) {
    return 1.F / 2;
  }

  return 1.F;
}
}  // namespace

// ---
void Links::Draw(const AppState& app_state) {
  const auto& diagram = app_state.project.GetDiagram();
  const auto fixed_pin = app_state.widgets.new_link.FindFixedPin(diagram);

  const auto flow_tree = flow::BuildFlowTree(diagram);
  const auto node_flows = flow::CalculateNodeFlows(flow_tree);

  for (const auto& link : diagram.GetLinks()) {
    const auto alpha = GetLinkAlpha(link, fixed_pin);

    const auto start_pin_node = core::FindPinNode(diagram, link.start_pin_id);
    const auto node_flow = node_flows.at(start_pin_node->GetId().Get());
    const auto start_pin_flow = flow::GetPinFlow(node_flow, link.start_pin_id);
    auto color = app_state.widgets.settings_view.GetColorForFlowValue(
        start_pin_flow, app_state.project.GetSettings());
    color.Value.w = alpha;

    ne::Link(link.id, link.start_pin_id, link.end_pin_id, color, 2.F);
  }

  DrawLinkBeingRepinned(diagram, app_state.widgets.new_link,
                        app_state.widgets.nodes);
}

void Links::DrawLinkBeingRepinned(const core::Diagram& diagram,
                                  const NewLink& new_link, const Nodes& nodes) {
  const auto fixed_pin = new_link.FindFixedPin(diagram);

  if (!fixed_pin.has_value()) {
    return;
  }

  const auto fixed_pin_rect = nodes.GetDrawnPinIconRect(*fixed_pin);
  const auto fixed_pin_node = FindPinNode(diagram, *fixed_pin);
  const auto fixed_pin_kind = core::GetPinKind(*fixed_pin_node, *fixed_pin);
  const auto fixed_pin_position =
      (fixed_pin_kind == ax::NodeEditor::PinKind::Input)
          ? ImVec2{fixed_pin_rect.Min.x,
                   (fixed_pin_rect.Min.y + fixed_pin_rect.Max.y) / 2}
          : ImVec2{fixed_pin_rect.Max.x,
                   (fixed_pin_rect.Min.y + fixed_pin_rect.Max.y) / 2};

  const auto curve =
      GetCurve(fixed_pin_position, ImGui::GetMousePos(), fixed_pin_kind);

  auto color = ImColor{1.F, 1.F, 1.F};

  if (const auto hovering_over_pin = new_link.GetHoveringOverPin()) {
    if (new_link.CanConnectToPin(*hovering_over_pin, diagram)) {
      color = ImColor{1.F / 2, 1.F, 1.F / 2};
    } else {
      color = ImColor{1.F, 1.F / 2, 1.F / 2};
    }
  }

  auto* draw_list = ImGui::GetWindowDrawList();
  Expects(draw_list != nullptr);
  draw_list->AddBezierCubic(curve.P0, curve.P1, curve.P2, curve.P3, color, 4.F);
}
}  // namespace esc::draw