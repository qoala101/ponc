/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#include "draw_links.h"

#include <imgui.h>

#include "app_events.h"
#include "core_diagram.h"
#include "core_i_node.h"
#include "cpp_assert.h"
#include "flow_tree.h"
#include "imgui_node_editor.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include "app_state.h"
#include "core_project.h"
#include "cpp_scope.h"
#include "draw_tooltip.h"
#include "draw_widgets.h"
#include "imgui_bezier_math.h"

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
  for (const auto& link : app_state.project->GetDiagram().GetLinks()) {
    const auto color = ImColor{255, 255, 255};
    ne::Link(link.id, link.start_pin_id, link.end_pin_id, color, 2.F);
  }

  DrawLinkBeingRepinned(app_state);
}

void Links::DrawLinkBeingRepinned(const AppState& app_state) {
  if (!app_state.widgets->new_link.IsVisible()) {
    return;
  }

  const auto dragged_from_pin = app_state.widgets->new_link.GetDraggedFromPin();
  const auto link_to_repin =
      FindPinLink(app_state.project->GetDiagram(), dragged_from_pin);

  if (!link_to_repin.has_value()) {
    return;
  }

  const auto fixed_pin_id = (dragged_from_pin == (*link_to_repin)->start_pin_id)
                                ? (*link_to_repin)->end_pin_id
                                : (*link_to_repin)->start_pin_id;
  const auto fixed_pin_kind = (fixed_pin_id == (*link_to_repin)->start_pin_id)
                                  ? ne::PinKind::Output
                                  : ne::PinKind::Input;
  const auto fixed_pin_position =
      app_state.widgets->nodes.GetPinPosition(fixed_pin_id);

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
  drawList->AddBezierCubic(curve.P0, curve.P1, curve.P2, curve.P3, color, 2.0F);
}
}  // namespace esc::draw