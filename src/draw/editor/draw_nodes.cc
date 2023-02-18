#include "draw_nodes.h"

#include "core_i_node.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include "app_node_drawer.h"
#include "app_state.h"
#include "coreui_i_node_drawer.h"
#include "cpp_scope.h"
#include "imgui.h"
#include "imgui_bezier_math.h"
#include "imgui_node_editor.h"

namespace esc::draw {
namespace {
// vh: ok
struct Xy {
  float x{};
  float y{};
};

void DrawFlowIcon(ImDrawList* drawList, const ImVec2& a, const ImVec2& b,
                  bool filled, ImU32 color, ImU32 innerColor) {
  auto rect = ImRect(a, b);
  auto rect_x = rect.Min.x;
  auto rect_y = rect.Min.y;
  auto rect_w = rect.Max.x - rect.Min.x;
  auto rect_h = rect.Max.y - rect.Min.y;
  auto rect_center_x = (rect.Min.x + rect.Max.x) * 0.5f;
  auto rect_center_y = (rect.Min.y + rect.Max.y) * 0.5f;
  auto rect_center = ImVec2(rect_center_x, rect_center_y);
  const auto outline_scale = rect_w / 24.0f;
  const auto extra_segments =
      static_cast<int>(2 * outline_scale);  // for full circle

  const auto origin_scale = rect_w / 24.0f;

  const auto offset_x = 1.0f * origin_scale;
  const auto offset_y = 0.0f * origin_scale;
  const auto margin = (filled ? 2.0f : 2.0f) * origin_scale;
  const auto rounding = 0.1f * origin_scale;
  const auto tip_round = 0.7f;  // percentage of triangle edge (for tip)
  // const auto edge_round = 0.7f; // percentage of triangle edge (for corner)
  const auto canvas =
      ImRect(rect.Min.x + margin + offset_x, rect.Min.y + margin + offset_y,
             rect.Max.x - margin + offset_x, rect.Max.y - margin + offset_y);
  const auto canvas_x = canvas.Min.x;
  const auto canvas_y = canvas.Min.y;
  const auto canvas_w = canvas.Max.x - canvas.Min.x;
  const auto canvas_h = canvas.Max.y - canvas.Min.y;

  const auto left = canvas_x + canvas_w * 0.5f * 0.3f;
  const auto right = canvas_x + canvas_w - canvas_w * 0.5f * 0.3f;
  const auto top = canvas_y + canvas_h * 0.5f * 0.2f;
  const auto bottom = canvas_y + canvas_h - canvas_h * 0.5f * 0.2f;
  const auto center_y = (top + bottom) * 0.5f;
  // const auto angle = AX_PI * 0.5f * 0.5f * 0.5f;

  const auto tip_top = ImVec2(canvas_x + canvas_w * 0.5f, top);
  const auto tip_right = ImVec2(right, center_y);
  const auto tip_bottom = ImVec2(canvas_x + canvas_w * 0.5f, bottom);

  drawList->PathLineTo(ImVec2(left, top) + ImVec2(0, rounding));
  drawList->PathBezierCubicCurveTo(ImVec2(left, top), ImVec2(left, top),
                                   ImVec2(left, top) + ImVec2(rounding, 0));
  drawList->PathLineTo(tip_top);
  drawList->PathLineTo(tip_top + (tip_right - tip_top) * tip_round);
  drawList->PathBezierCubicCurveTo(
      tip_right, tip_right, tip_bottom + (tip_right - tip_bottom) * tip_round);
  drawList->PathLineTo(tip_bottom);
  drawList->PathLineTo(ImVec2(left, bottom) + ImVec2(rounding, 0));
  drawList->PathBezierCubicCurveTo(ImVec2(left, bottom), ImVec2(left, bottom),
                                   ImVec2(left, bottom) - ImVec2(0, rounding));

  if (!filled) {
    if (innerColor & 0xFF000000)
      drawList->AddConvexPolyFilled(drawList->_Path.Data, drawList->_Path.Size,
                                    innerColor);

    drawList->PathStroke(color, true, 2.0f * outline_scale);
  } else
    drawList->PathFillConvex(color);
}

void Icon(const ImVec2& size, bool filled,
          const ImVec4& color /* = ImVec4(1, 1, 1, 1)*/,
          const ImVec4& innerColor /* = ImVec4(0, 0, 0, 0)*/) {
  if (ImGui::IsRectVisible(size)) {
    auto cursorPos = ImGui::GetCursorScreenPos();
    auto drawList = ImGui::GetWindowDrawList();
    DrawFlowIcon(drawList, cursorPos, cursorPos + size, filled, ImColor(color),
                 ImColor(innerColor));
  }

  ImGui::Dummy(size);
}
auto IsPinLinked(State& state, ne::PinId id) -> bool {
  if (!id) return false;

  for (const auto& link : state.core_state->diagram_.GetLinks())
    if (link.start_pin_id == id || link.end_pin_id == id) return true;

  return false;
}
// vh: norm
void DrawPinIcon(State& state, core::INode& node,
                 coreui::IPinDrawer& pin_drawer, float alpha) {
  // const auto& new_link = state.draw_state->new_link;
  // if (new_link.has_value()) {
  //   if (pin_id == new_link->pin_dragged_from) {
  //     ImGui::GetWindowDrawList()->AddCircle(ImGui::GetCursorPos(), 5.0F,
  //                                           ImColor{255, 0, 0}, 0, 3.0F);
  //   }

  //   if (new_link->pin_hovered_over.has_value()) {
  //     if (pin_id == *new_link->pin_hovered_over) {
  //       ImGui::GetWindowDrawList()->AddCircle(ImGui::GetCursorPos(), 7.0F,
  //                                             ImColor{0, 255, 0}, 0, 3.0F);
  //     }
  //   }

  //   if (new_link->rebind.has_value()) {
  //     if (pin_id == new_link->rebind->fixed_pin) {
  //       ImGui::GetWindowDrawList()->AddCircle(ImGui::GetCursorPos(), 7.0F,
  //                                             ImColor{0, 0, 255}, 0, 3.0F);
  //     }
  //   }
  // }

  const auto pin_id = pin_drawer.GetPinId();

  if (!pin_id.has_value()) {
    if (pin_drawer.IsEditable()) {
      ImGui::Dummy(ImVec2{20, 24});
    } else {
      ImGui::Dummy(ImVec2{24, 24});
    }

    return;
  }

  const auto size = ImVec2{24, 24};

  const auto node_flow =
      state.core_state->flow_calculator_.GetCalculatedFlow(node);
  auto color = ImColor{255, 255, 255};

  if (state.core_state->flow_colors_.color_flow) {
    const auto node_flow =
        state.core_state->flow_calculator_.GetCalculatedFlow(node);
    const auto flow = GetPinFlow(node_flow, *pin_id);
    color = state.core_state->flow_colors_.GetColorForFlowValue(flow);
  }

  color.Value.w = static_cast<int>(alpha * 255);

  const auto stroke_color = color;
  const auto fill_color = ImColor{32, 32, 32, static_cast<int>(alpha * 255)};

  const auto connected = IsPinLinked(state, *pin_id);

  Icon(size, connected, stroke_color, fill_color);
}
// vh: bad
void DrawPinField(coreui::IPinDrawer& pin_drawer) {
  if (auto* float_value = pin_drawer.GetFloat()) {
    ImGui::Spring(0);

    if (pin_drawer.IsEditable()) {
      ImGui::SetNextItemWidth(100);
      ImGui::InputFloat(pin_drawer.GetLabel().c_str(), float_value, 0.0F, 0.0F,
                        "%.3f");
    } else {
      ImGui::Text("%.3f %s", *float_value, pin_drawer.GetLabel().c_str());
    }

    ImGui::Spring(0);
  } else {
    if (!pin_drawer.GetLabel().empty()) {
      ImGui::Spring(0);
      ImGui::TextUnformatted(pin_drawer.GetLabel().c_str());
      ImGui::Spring(0);
    }
  }
}
// vh: bad
void DrawNodeHeader(coreui::INodeDrawer& node_drawer) {
  ImGui::Spring(0);
  ImGui::TextUnformatted(node_drawer.GetLabel().c_str());
  ImGui::Spring(1);
  ImGui::Dummy(ImVec2{0, 28});
  ImGui::Spring(0);
}

auto IsFlowPin(ne::PinId id, const core::INode& node) -> bool {
  const auto flow_values = node.GetInitialFlow();

  if (flow_values.input_pin_flow.has_value() &&
      flow_values.input_pin_flow->first == id.Get()) {
    return true;
  }

  return std::ranges::find_if(flow_values.output_pin_flows,
                              [id](const auto& flow_value) {
                                return flow_value.first == id.Get();
                              }) != flow_values.output_pin_flows.end();
}

// vh: bad
auto CalculateAlphaForPin(State& state, std::optional<ne::PinId> pin_id) {
  auto alpha = ImGui::GetStyle().Alpha;

  // if (state.draw_state->not_yet_connected_pin_of_new_link_id.has_value() &&
  //     !CanCreateLink(*state.draw_state->not_yet_connected_pin_of_new_link_id,
  //                    pin_id) &&
  //     (pin_id != *state.draw_state->not_yet_connected_pin_of_new_link_id))
  //     {
  //   alpha = alpha * (48.0F / 255.0F);
  // }

  // if (new_link_->has_value()) {
  //   if
  //   (!state.CanConnectFromPinToPin((*new_link_)->pin_dragged_from,
  //                                     pin_id)) {
  //     alpha = alpha * (48.0F / 255.0F);
  //   }
  // }
  // alpha = alpha * (48.0F / 255.0F);

  // if (state.draw_state->not_yet_connected_pin_of_new_link_id.has_value() &&
  //     (pin_id != *state.draw_state->not_yet_connected_pin_of_new_link_id)) {
  //   alpha = alpha * (48.0F / 255.0F);
  // }

  return alpha;
}
}  // namespace

Nodes::Nodes(const Texture& node_header_texture,
             std::shared_ptr<std::optional<NewLink>> new_link)
    : node_header_texture_{node_header_texture},
      new_link_{std::move(new_link)} {}

void Nodes::Draw(State& state) {
  for (const auto& family : state.core_state->diagram_.GetFamilies()) {
    for (const auto& node : family->GetNodes()) {
      DrawNode(state, *node);
    }
  }
}

void Nodes::DrawNode(State& state, core::INode& node) {
  auto node_builder = esc::NodeDrawer{node.GetId()};
  auto drawer = node.CreateDrawer(state.ToStateNoQueue());

  if (drawer->HasHeader()) {
    const auto header_texture = node_header_texture_;

    {
      auto color = drawer->GetColor();

      if (state.core_state->flow_colors_.color_flow) {
        const auto node_flow =
            state.core_state->flow_calculator_.GetCalculatedFlow(node);

        if (node_flow.input_pin_flow.has_value()) {
          const auto flow =
              GetPinFlow(node_flow, node_flow.input_pin_flow->first);
          color = state.core_state->flow_colors_.GetColorForFlowValue(flow);
        } else {
          color = ImColor{255, 255, 255};
        }
      }

      const auto header_scope = node_builder.AddHeader(header_texture, color);

      DrawNodeHeader(*node.CreateDrawer(state.ToStateNoQueue()));
    }
  }

  for (const auto& pin_drawer :
       node.CreateDrawer(state.ToStateNoQueue())->CreatePinDrawers()) {
    const auto pin_id = pin_drawer->GetPinId();
    const auto pin_kind = pin_drawer->GetKind();
    const auto input_scope = node_builder.AddPin(pin_id, pin_kind);
    const auto alpha = CalculateAlphaForPin(state, pin_id);

    if (pin_kind == ne::PinKind::Input) {
      DrawPinIcon(state, node, *pin_drawer, alpha);
      DrawPinField(*pin_drawer);
    } else {
      DrawPinField(*pin_drawer);
      DrawPinIcon(state, node, *pin_drawer, alpha);
    }

    // state.draw_state->pin_poses_[pin_id.Get()] = ImGui::GetItemRectMin();

    if (new_link_->has_value()) {
      if ((*new_link_)->rebind.has_value()) {
        if ((*new_link_)->rebind->fixed_pin == pin_id) {
          const auto rect =
              ImRect{ImGui::GetItemRectMin(), ImGui::GetItemRectMax()};

          const auto end_pos =
              (pin_kind == ax::NodeEditor::PinKind::Input)
                  ? ImVec2{rect.Min.x, (rect.Min.y + rect.Max.y) * 0.5f}
                  : ImVec2{rect.Max.x, (rect.Min.y + rect.Max.y) * 0.5f};

          (*new_link_)->rebind->fixed_pin_pos = end_pos;
        }
      }
    }

    // drawList->AddLine(ImGui::GetItemRectMin(), ImGui::GetMousePos(),
    //                   ImColor{55, 0, 255}, 2);
    // drawList->AddLine(ImGui::GetItemRectMax(), ImGui::GetMousePos(),
    //                   ImColor{77, 0, 255}, 2);
  }
}
}  // namespace esc::draw