#include "esc_app.h"

#include <application.h>
#include <imgui_node_editor.h>
#include <sys/types.h>

#include <chrono>
#include <ios>
#include <memory>

#include "core_diagram.h"
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
void DrawPinIcon(draw::IPinDrawer& pin_drawer, bool connected, float alpha) {
  if (!pin_drawer.IsConnectable()) {
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
void DrawNodeHeader(core::INode& node) {
  ImGui::Spring(0);
  ImGui::TextUnformatted(node.CreateDrawer()->GetLabel().c_str());
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
// vh: norm
void DrawHintLabel(const char* label, const ImColor& color) {
  ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());

  const auto& style = ImGui::GetStyle();
  const auto spacing = style.ItemSpacing;

  ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2{spacing.x, -spacing.y});

  const auto padding = style.FramePadding;
  const auto rect_min = ImGui::GetCursorScreenPos() - padding;
  const auto text_size = ImGui::CalcTextSize(label);
  const auto rect_max = ImGui::GetCursorScreenPos() + text_size + padding;

  auto* drawList = ImGui::GetWindowDrawList();
  drawList->AddRectFilled(rect_min, rect_max, color, text_size.y * 0.15F);

  ImGui::TextUnformatted(label);
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
}
// vh: norm
void App::OnStop() {
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
      (*state_)->app_.GetDiagram().FindPin(pin_id)->GetKind() ==
      ne::PinKind::Output;
  const auto link = core::Link{
      .id = link_id,
      .start_pin_id =
          is_link_starts_on_existing_node ? pin_id : *matching_node_pin_id,
      .end_pin_id =
          is_link_starts_on_existing_node ? *matching_node_pin_id : pin_id};

  (*state_)->app_.GetDiagram().EmplaceLink(link);
}

auto App::IsPinLinked(ne::PinId id) const -> bool {
  if (!id) return false;

  for (const auto& link : (*state_)->app_.GetDiagram().GetLinks())
    if (link.start_pin_id == id || link.end_pin_id == id) return true;

  return false;
}

// vh: bad
void App::DrawContextMenuProcess() {
  const auto open_popup_pos = ImGui::GetMousePos();

  {
    const auto suspend_scope =
        cpp::Scope{[]() { ne::Suspend(); }, []() { ne::Resume(); }};

    if (ne::ShowLinkContextMenu(&popup_state_.context_link_id)) {
      ImGui::OpenPopup("Link Context Menu");
    } else if (ne::ShowNodeContextMenu(&popup_state_.context_node_id)) {
      ImGui::OpenPopup("Node Context Menu");
    } else if (ne::ShowPinContextMenu(&popup_state_.context_pin_id)) {
      ImGui::OpenPopup("Pin Context Menu");
    } else if (ne::ShowBackgroundContextMenu()) {
      drawing_state_.connect_new_node_to_existing_pin_id.reset();
      ImGui::OpenPopup("Create New Node");
    }
  }

  {
    const auto suspend_scope =
        cpp::Scope{[]() { ne::Suspend(); }, []() { ne::Resume(); }};

    {
      const auto style_var_scope = cpp::Scope{
          []() {
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
          },
          []() { ImGui::PopStyleVar(); }};

      if (ImGui::BeginPopup("Link Context Menu")) {
        const auto popup_scope = cpp::Scope{[]() { ImGui::EndPopup(); }};

        ImGui::TextUnformatted("Link");
        ImGui::Separator();

        const auto& link = (*state_)->app_.GetDiagram().FindLink(
            popup_state_.context_link_id);

        ImGui::Text("ID: %p", link.id.AsPointer());
        ImGui::Text("From: %p", link.start_pin_id.AsPointer());
        ImGui::Text("To: %p", link.end_pin_id.AsPointer());

        ImGui::Separator();

        if (ImGui::MenuItem("Delete")) {
          ne::DeleteLink(popup_state_.context_link_id);
        }
      }

      if (ImGui::BeginPopup("Node Context Menu")) {
        const auto popup_scope = cpp::Scope{[]() { ImGui::EndPopup(); }};

        ImGui::TextUnformatted("Node");
        ImGui::Separator();

        auto& node = (*state_)->app_.GetDiagram().FindNode(
            popup_state_.context_node_id);

        ImGui::Text("ID: %p", node.GetId().AsPointer());
        ImGui::Text("Type: %s", node.CreateDrawer()->GetLabel().c_str());

        ImGui::Separator();

        if (ImGui::MenuItem("Delete")) {
          ne::DeleteNode(popup_state_.context_node_id);
        }
      }

      if (ImGui::BeginPopup("Pin Context Menu")) {
        const auto popup_scope = cpp::Scope{[]() { ImGui::EndPopup(); }};

        ImGui::TextUnformatted("Pin");
        ImGui::Separator();

        ImGui::Text("ID: %p", popup_state_.context_pin_id.AsPointer());
      }

      if (ImGui::BeginPopup("Create New Node")) {
        const auto popup_scope = cpp::Scope{[]() { ImGui::EndPopup(); }};

        ImGui::TextUnformatted("Create New Node");
        ImGui::Separator();

        for (const auto& family : (*state_)->app_.GetDiagram().GetFamilies()) {
          if (ImGui::MenuItem(family->CreateDrawer()->GetLabel().c_str())) {
            auto& new_node = family->EmplaceNode(*(*state_)->id_generator_);

            new_node.SetPosition(open_popup_pos);

            if (const auto node_created_by_link_from_existing_one =
                    drawing_state_.connect_new_node_to_existing_pin_id
                        .has_value()) {
              AddLinkFromPinToNode(
                  (*state_)->id_generator_->GetNext<ne::LinkId>(),
                  *drawing_state_.connect_new_node_to_existing_pin_id,
                  new_node);
            }

            break;
          }
        }
      }
    }
  }
}
// vh: ok
auto App::CanCreateLink(ne::PinId left, ne::PinId right) -> bool {
  return (left != right) &&
         ((*state_)->app_.GetDiagram().FindPin(left)->GetKind() !=
          (*state_)->app_.GetDiagram().FindPin(right)->GetKind());
}
// vh: bad
auto App::CalculateAlphaForPin(ne::PinId pin_id) {
  auto alpha = ImGui::GetStyle().Alpha;

  if (drawing_state_.not_yet_connected_pin_of_new_link_id.has_value() &&
      !CanCreateLink(*drawing_state_.not_yet_connected_pin_of_new_link_id,
                     pin_id) &&
      (pin_id != *drawing_state_.not_yet_connected_pin_of_new_link_id)) {
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
    const auto header_scope =
        node_builder.AddHeader(header_texture, node.CreateDrawer()->GetColor());

    DrawNodeHeader(node);
  }

  for (const auto pin_id : node.GetPinIds()) {
    auto drawer = (*state_)->app_.GetDiagram().FindPin(pin_id);
    const auto kind = drawer->GetKind();
    const auto alpha = CalculateAlphaForPin(pin_id);

    if (kind == ne::PinKind::Input) {
      const auto input_scope = node_builder.AddPin(pin_id, kind);

      {
        const auto style_var_scope = cpp::Scope{
            [alpha]() { ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha); },
            []() { ImGui::PopStyleVar(); }};

        DrawPinIcon(*drawer, IsPinLinked(pin_id), alpha);
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
        DrawPinIcon(*drawer, IsPinLinked(pin_id), alpha);
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
void App::DrawLinkConnectionProcess() {
  {
    const auto create_scope = cpp::Scope{[]() { ne::EndCreate(); }};

    if (ne::BeginCreate(ImColor{255, 255, 255}, 2.0F)) {
      auto start_pin_id = ne::PinId{};
      auto end_pin_id = ne::PinId{};

      if (ne::QueryNewLink(&start_pin_id, &end_pin_id)) {
        drawing_state_.not_yet_connected_pin_of_new_link_id = start_pin_id;

        auto start_pin_drawer =
            (*state_)->app_.GetDiagram().FindPin(start_pin_id);
        auto end_pin_drawer = (*state_)->app_.GetDiagram().FindPin(end_pin_id);

        if (start_pin_drawer->GetKind() == ne::PinKind::Input) {
          using std::swap;

          swap(start_pin_drawer, end_pin_drawer);
          swap(start_pin_id, end_pin_id);
        }

        if (end_pin_id == start_pin_id) {
          ne::RejectNewItem(ImColor{255, 0, 0}, 2.0F);
        } else if (end_pin_drawer->GetKind() == start_pin_drawer->GetKind()) {
          DrawHintLabel("x Incompatible Pin Kind", ImColor{45, 32, 32, 180});
          ne::RejectNewItem(ImColor{255, 0, 0}, 2.0F);
          // } else if (end_pin->ui_data_.node ==
          // start_pin_drawer->ui_data_.node) {
          //   DrawHintLabel("x Cannot connect to self", ImColor{45, 32, 32,
          //   180}); ne::RejectNewItem(ImColor{255, 0, 0}, 1.0F);
          // } else if (end_pin->GetType() != start_pin_drawer->GetType()) {
          //   DrawHintLabel("x Incompatible Pin Type", ImColor{45, 32, 32,
          //   180}); ne::RejectNewItem(ImColor{255, 127, 127}, 1.0F);
        } else {
          DrawHintLabel("+ Create Link", ImColor{32, 45, 32, 180});

          if (ne::AcceptNewItem(ImColor{127, 255, 127}, 4.0F)) {
            (*state_)->app_.GetDiagram().EmplaceLink(
                core::Link{(*state_)->id_generator_->GetNext<ne::LinkId>(),
                           start_pin_id, end_pin_id});
          }
        }
      }

      if (ne::QueryNewNode(&end_pin_id)) {
        drawing_state_.not_yet_connected_pin_of_new_link_id = end_pin_id;

        DrawHintLabel("+ Create Node", ImColor{32, 45, 32, 180});

        if (ne::AcceptNewItem()) {
          drawing_state_.connect_new_node_to_existing_pin_id =
              drawing_state_.not_yet_connected_pin_of_new_link_id;
          drawing_state_.not_yet_connected_pin_of_new_link_id.reset();

          {
            const auto suspend_scope =
                cpp::Scope{[]() { ne::Suspend(); }, []() { ne::Resume(); }};

            ImGui::OpenPopup("Create New Node");
          }
        }
      }
    } else {
      drawing_state_.not_yet_connected_pin_of_new_link_id.reset();
    }
  }
}
// vh: norm
void App::DrawDeleteItemsProcess() {
  const auto delete_scope = cpp::Scope{[]() { ne::EndDelete(); }};

  if (ne::BeginDelete()) {
    auto link_id = ne::LinkId{};

    while (ne::QueryDeletedLink(&link_id)) {
      if (ne::AcceptDeletedItem()) {
        (*state_)->app_.GetDiagram().EraseLink(link_id);
      }
    }

    auto node_id = ne::NodeId{};

    while (ne::QueryDeletedNode(&node_id)) {
      if (ne::AcceptDeletedItem()) {
        (*state_)->app_.GetDiagram().EraseNode(node_id);
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
  DrawLinkConnectionProcess();
  DrawDeleteItemsProcess();
  DrawContextMenuProcess();
}
// vh: bad
void App::DrawFrame() {
  main_window_->Draw(**state_);
  DrawNodeEditor();

  ImGui::ShowDemoWindow();
  ImGui::ShowMetricsWindow();
}
}  // namespace esc