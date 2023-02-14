#include "draw_links.h"
#include "imgui_bezier_math.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include "imgui.h"

namespace esc::draw {
namespace {
auto GetCurve(const ImVec2& m_Start, const ImVec2& m_End) {
  const auto strength = ne::GetStyle().LinkStrength;
  const auto start_dir = ne::GetStyle().SourceDirection;
  const auto end_dir = ne::GetStyle().TargetDirection;
  // m_CurrentPin->m_Dir = kind == PinKind::Output ? editorStyle.SourceDirection
  //                                               :
  //                                               editorStyle.TargetDirection;

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

void Links::Draw(State& state) {
  const auto draw_line_from_pin = state.GetDrawLINEFromPin();
  const auto* existing_link_from_same_pin = state.GetExistingLinkFromSamePin();

  for (const auto& link : state.app_.GetDiagram().GetLinks()) {
    // if (state.drawing_.not_yet_connected_pin_of_new_link_id.has_value()) {
    //   const auto pin_id =
    //   *state.drawing_.not_yet_connected_pin_of_new_link_id; const auto& node
    //   = state.app_.GetDiagram().FindPinNode(pin_id);

    //   // std::cout << node->GetId().Get() << "\n";

    //   if (pin_id == link.start_pin_id || pin_id == link.end_pin_id) {
    //     ne::Link(link.id, link.start_pin_id, link.end_pin_id,
    //              ImColor{255, 0, 0}, 0.0F);

    //     continue;
    //   }
    // }

    if (&link != existing_link_from_same_pin) {
      ne::Link(link.id, link.start_pin_id, link.end_pin_id,
               ImColor{255, 255, 255}, 2.0F);
    }
  }

  if (!draw_line_from_pin.has_value()) {
    return;
  }

  auto* drawList = ImGui::GetWindowDrawList();
  const auto curve = GetCurve(state.drawing_.pinned_pin_pos, ImGui::GetMousePos());
  drawList->AddBezierCubic(curve.P0, curve.P1, curve.P2, curve.P3,
                           ImColor{255, 255, 255}, 2.0F);
}
}  // namespace esc::draw