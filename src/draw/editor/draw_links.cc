/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>

#include "draw_links.h"


#include "app_events.h"
#include "app_state.h"
#include "core_diagram.h"
#include "core_i_node.h"
#include "core_project.h"
#include "cpp_assert.h"
#include "cpp_scope.h"
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
}  // namespace

// ---
void Links::Draw(const AppState& app_state) {
  for (const auto& link : app_state.project.GetDiagram().GetLinks()) {
    const auto color = ImColor{255, 255, 255};
    ne::Link(link.id, link.start_pin_id, link.end_pin_id, color, 2.F);
  }

  DrawLinkBeingRepinned(app_state.project.GetDiagram(),
                        app_state.widgets.new_link, app_state.widgets.nodes);
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

  auto color = ImColor{255, 255, 255};

  // if (new_link_->pin_hovered_over.has_value()) {
  //   // if (!CanConnectFromPinToPin(*state.core_state, *new_link_,
  //   //                             new_link_->pin_dragged_from,
  //   //                             *new_link_->pin_hovered_over)) {
  //   //   color = ImColor{255, 0, 0};
  //   // }
  // }

  auto* drawList = ImGui::GetWindowDrawList();
  drawList->AddBezierCubic(curve.P0, curve.P1, curve.P2, curve.P3, color, 4.F);
}
}  // namespace esc::draw