#include "esc_app.h"

#include <application.h>
#include <bits/ranges_algo.h>
#include <imgui_node_editor.h>
#include <sys/types.h>

#include <chrono>
#include <ios>
#include <memory>
#include <unordered_map>

#include "core_diagram.h"
#include "core_i_family.h"
#include "core_i_node.h"
#include "core_id_generator.h"
#include "cpp_assert.h"
#include "cpp_scope.h"
#include "draw_i_family_drawer.h"
#include "draw_i_node_drawer.h"
#include "draw_i_pin_drawer.h"
#include "esc_node_drawer.h"
#include "esc_state.h"
#include "esc_types.h"
#include "imgui.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imfilebrowser.h>
#include <imgui_internal.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace ne = ax::NodeEditor;

namespace esc {
namespace {
// vh: ok
auto GetItemRect() {
  return ImRect{ImGui::GetItemRectMin(), ImGui::GetItemRectMax()};
}
// vh: ok
struct Xy {
  float x{};
  float y{};
};
// vh: ok
auto GetExpandedRect(const ImRect& rect, const Xy& xy) {
  auto result = rect;
  result.Min.x -= xy.x;
  result.Min.y -= xy.y;
  result.Max.x += xy.x;
  result.Max.y += xy.y;
  return result;
}
// vh: norm
void DrawSplitter(float thickness, float* left_size, float* right_size,
                  float min_left_size, float min_right_size) {
  cpp::Expects(left_size != nullptr);
  cpp::Expects(GImGui != nullptr);

  const auto id = GImGui->CurrentWindow->GetID("##Splitter");
  const auto rect_min =
      GImGui->CurrentWindow->DC.CursorPos + ImVec2{*left_size, 0.0F};
  const auto rect_max =
      rect_min + ImGui::CalcItemSize(ImVec2{thickness, -1.0F}, 0.0F, 0.0F);
  const auto rect = ImRect{rect_min, rect_max};

  ImGui::SplitterBehavior(rect, id, ImGuiAxis_X, left_size, right_size,
                          min_left_size, min_right_size, 0.0F);
}

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
// vh: norm
void DrawPinIcon(draw::IPinDrawer& pin_drawer, bool connectable, bool connected,
                 float alpha) {
  if (!connectable) {
    if (pin_drawer.IsEditable()) {
      ImGui::Dummy(ImVec2{20, 24});
    } else {
      ImGui::Dummy(ImVec2{24, 24});
    }

    return;
  }

  const auto size = ImVec2{24, 24};
  const auto stroke_color =
      ImColor{255, 255, 255, static_cast<int>(alpha * 255)};
  const auto fill_color = ImColor{32, 32, 32, static_cast<int>(alpha * 255)};

  Icon(size, connected, stroke_color, fill_color);
}
// vh: bad
void DrawPinField(draw::IPinDrawer& pin_drawer) {
  ImGui::Spring(0);

  if (auto* float_value = pin_drawer.GetFloat()) {
    if (pin_drawer.IsEditable()) {
      ImGui::SetNextItemWidth(100);
      ImGui::InputFloat(pin_drawer.GetLabel().c_str(), float_value, 0.0F, 0.0F,
                        "%.3f");
    } else {
      ImGui::Text("%.3f %s", *float_value, pin_drawer.GetLabel().c_str());
    }
  } else {
    if (!pin_drawer.GetLabel().empty()) {
      ImGui::TextUnformatted(pin_drawer.GetLabel().c_str());
    }
  }

  ImGui::Spring(0);
}
// vh: bad
void DrawNodeHeader(draw::INodeDrawer& node_drawer) {
  ImGui::Spring(0);
  ImGui::TextUnformatted(node_drawer.GetLabel().c_str());
  ImGui::Spring(1);
  ImGui::Dummy(ImVec2{0, 28});
  ImGui::Spring(0);

  // if (auto* coupler_node = dynamic_cast<esc::CouplerNode*>(&node)) {
  //   ImGui::SetNextItemWidth(100);
  //   const auto& coupler_percentage_names = GetCouplerPercentageNames();
  //   ImGui::SliderInt(
  //       "", &coupler_node->GetCouplerPercentageIndex(), 0,
  //       static_cast<int>(coupler_percentage_names.size()) - 1,
  //       coupler_percentage_names[coupler_node->GetCouplerPercentageIndex()]
  //           .c_str());
  //   ImGui::Spring(0);
  // }
}
}  // namespace
// vh: norm
App::App(const char* name, int argc, char** argv)
    : Application{name, argc, argv} {}
// vh: ok
auto App::GetTextures() -> esc::TexturesHandle& { return *textures_; }
// vh: norm
void App::OnStart() {
  state_.emplace(std::make_shared<State>());
  editor_context_.emplace();
  textures_.emplace(shared_from_this());
  main_window_.emplace();
  popups_.emplace();
  link_connection_process_.emplace();
}
// vh: norm
void App::OnStop() {
  link_connection_process_.reset();
  popups_.reset();
  main_window_.reset();
  textures_.reset();
  editor_context_.reset();
  state_.reset();
}

auto App::GetWindowFlags() const -> ImGuiWindowFlags {
  return Application::GetWindowFlags() | ImGuiWindowFlags_MenuBar;
}

// vh: ok
void App::OnFrame(float /*unused*/) {
  (*state_)->OnFrame();
  DrawFrame();
}

void App::AddLinkFromPinToNode(ne::LinkId link_id, ne::PinId pin_id,
                               core::INode& node) {
  const auto& node_pin_ids = node.GetPinIds();
  const auto matching_node_pin_id =
      std::ranges::find_if(node_pin_ids, [this, pin_id](auto node_pin_id) {
        return CanCreateLink(pin_id, node_pin_id);
      });

  if (matching_node_pin_id == node_pin_ids.end()) {
    return;
  }

  const auto is_link_starts_on_existing_node =
      (*state_)->app_.GetDiagram().FindPin(pin_id, **state_)->GetKind() ==
      ne::PinKind::Output;
  const auto link = core::Link{
      .id = link_id,
      .start_pin_id =
          is_link_starts_on_existing_node ? pin_id : *matching_node_pin_id,
      .end_pin_id =
          is_link_starts_on_existing_node ? *matching_node_pin_id : pin_id};

  (*state_)->app_.GetDiagram().EmplaceLink(link);
}

auto App::IsFlowPin(ne::PinId id, const core::INode& node) const -> bool {
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

auto App::IsPinLinked(ne::PinId id) const -> bool {
  if (!id) return false;

  for (const auto& link : (*state_)->app_.GetDiagram().GetLinks())
    if (link.start_pin_id == id || link.end_pin_id == id) return true;

  return false;
}

// vh: ok
auto App::CanCreateLink(ne::PinId left, ne::PinId right) -> bool {
  return (left != right) &&
         ((*state_)->app_.GetDiagram().FindPin(left, **state_)->GetKind() !=
          (*state_)->app_.GetDiagram().FindPin(right, **state_)->GetKind());
}
// vh: bad
auto App::CalculateAlphaForPin(ne::PinId pin_id) {
  auto alpha = ImGui::GetStyle().Alpha;

  if ((*state_)->drawing_.not_yet_connected_pin_of_new_link_id.has_value() &&
      !CanCreateLink(*(*state_)->drawing_.not_yet_connected_pin_of_new_link_id,
                     pin_id) &&
      (pin_id != *(*state_)->drawing_.not_yet_connected_pin_of_new_link_id)) {
    alpha = alpha * (48.0F / 255.0F);
  }

  return alpha;
}
// vh: bad
void App::DrawNode(core::INode& node) {
  auto node_builder = esc::NodeDrawer{node.GetId()};

  const auto header_texture =
      textures_->GetTextureWithDims(textures_->GetTextureIds().node_header);

  {
    const auto header_scope = node_builder.AddHeader(
        header_texture, node.CreateDrawer(**state_)->GetColor());

    DrawNodeHeader(*node.CreateDrawer(**state_));
  }

  for (const auto pin_id : node.GetPinIds()) {
    auto drawer = (*state_)->app_.GetDiagram().FindPin(pin_id, **state_);
    const auto kind = drawer->GetKind();
    const auto alpha = CalculateAlphaForPin(pin_id);

    if (kind == ne::PinKind::Input) {
      const auto input_scope = node_builder.AddPin(pin_id, kind);

      {
        const auto style_var_scope = cpp::Scope{
            [alpha]() { ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha); },
            []() { ImGui::PopStyleVar(); }};

        DrawPinIcon(*drawer, IsFlowPin(pin_id, node), IsPinLinked(pin_id),
                    alpha);
        DrawPinField(*drawer);
      }
    } else {
      const auto style_var_scope = cpp::Scope{
          [alpha]() { ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha); },
          []() { ImGui::PopStyleVar(); }};

      {
        const auto input_scope =
            node_builder.AddPin(pin_id, ne::PinKind::Output);

        DrawPinField(*drawer);
        DrawPinIcon(*drawer, IsFlowPin(pin_id, node), IsPinLinked(pin_id),
                    alpha);
      }
    }
  }
}
// vh: norm
void App::DrawNodes() {
  for (const auto& family : (*state_)->app_.GetDiagram().GetFamilies()) {
    for (const auto& node : family->GetNodes()) {
      DrawNode(*node);
    }
  }
}
// vh: norm
void App::DrawLinks() {
  for (const auto& link : (*state_)->app_.GetDiagram().GetLinks()) {
    ne::Link(link.id, link.start_pin_id, link.end_pin_id,
             ImColor{255, 255, 255}, 2.0F);
  }
}
// vh: norm
void App::DrawDeleteItemsProcess() {
  const auto delete_scope = cpp::Scope{[]() { ne::EndDelete(); }};

  if (ne::BeginDelete()) {
    auto link_id = ne::LinkId{};

    while (ne::QueryDeletedLink(&link_id)) {
      if (ne::AcceptDeletedItem()) {
      }
    }

    auto node_id = ne::NodeId{};

    while (ne::QueryDeletedNode(&node_id)) {
      if (ne::AcceptDeletedItem()) {
      }
    }
  }
}
// vh: norm
void App::DrawNodeEditor() {
  const auto node_editor_scope =
      cpp::Scope{[]() { ne::Begin("Node editor"); }, []() { ne::End(); }};

  DrawNodes();
  DrawLinks();
  link_connection_process_->Draw(**state_);
  DrawDeleteItemsProcess();
  popups_->Draw(**state_);
}
// vh: bad
void App::DrawFrame() {
  main_window_->Draw(**state_);
  DrawNodeEditor();

  ImGui::ShowDemoWindow();
  ImGui::ShowMetricsWindow();
}
}  // namespace esc