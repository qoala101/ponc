#include <iostream>

#include "imgui.h"
#include "imgui_bezier_math.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include "cpp_scope.h"
#include "draw_i_node_drawer.h"
#include "draw_link_connection_process.h"
#include "draw_repin_curve.h"
#include "draw_tooltip.h"
#include "imgui_bezier_math.h"
#include "imgui_node_editor.h"
#include "imgui_node_editor_internal.h"

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

void RepinCurve::Draw(State& state) {
  if (state.drawing_.new_link.has_value()) {
    if (state.drawing_.new_link->rebind.has_value()) {
      if (state.drawing_.new_link->rebind->fixed_pin_pos.has_value()) {
        const auto curve =
            GetCurve(*state.drawing_.new_link->rebind->fixed_pin_pos,
                     ImGui::GetMousePos(),
                     state.drawing_.new_link->rebind->fixed_pin_kind);

        auto color = ImColor{255, 255, 255};

        if (state.drawing_.new_link->pin_hovered_over.has_value()) {
          if (!state.CanConnectFromPinToPin(
                  state.drawing_.new_link->pin_dragged_from,
                  *state.drawing_.new_link->pin_hovered_over)) {
            color = ImColor{255, 0, 0};
          }
        }

        auto* drawList = ImGui::GetWindowDrawList();
        drawList->AddBezierCubic(curve.P0, curve.P1, curve.P2, curve.P3, color,
                                 2.0F);
      }
    }
  }
}
}  // namespace esc::draw