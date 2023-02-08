#include "esc_app.h"

#include <application.h>
#include <imgui_node_editor.h>
#include <sys/types.h>

#include <memory>

#include "esc_builders.h"
#include "esc_cpp.h"
#include "esc_enums.h"
#include "esc_nodes_and_links.h"
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
namespace util = ax::NodeEditor::Utilities;

namespace {
// vh: ok
auto GetItemRect [[nodiscard]] () {
  return ImRect{ImGui::GetItemRectMin(), ImGui::GetItemRectMax()};
}
// vh: ok
struct Xy {
  float x{};
  float y{};
};
// vh: ok
auto GetExpandedRect [[nodiscard]] (const ImRect& rect, const Xy& xy) {
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

void DrawIcon(ImDrawList* drawList, const ImVec2& a, const ImVec2& b,
              IconType type, bool filled, ImU32 color, ImU32 innerColor) {
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

  if (type == IconType::Flow) {
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
        tip_right, tip_right,
        tip_bottom + (tip_right - tip_bottom) * tip_round);
    drawList->PathLineTo(tip_bottom);
    drawList->PathLineTo(ImVec2(left, bottom) + ImVec2(rounding, 0));
    drawList->PathBezierCubicCurveTo(
        ImVec2(left, bottom), ImVec2(left, bottom),
        ImVec2(left, bottom) - ImVec2(0, rounding));

    if (!filled) {
      if (innerColor & 0xFF000000)
        drawList->AddConvexPolyFilled(drawList->_Path.Data,
                                      drawList->_Path.Size, innerColor);

      drawList->PathStroke(color, true, 2.0f * outline_scale);
    } else
      drawList->PathFillConvex(color);
  }
}

void Icon(const ImVec2& size, IconType type, bool filled,
          const ImVec4& color /* = ImVec4(1, 1, 1, 1)*/,
          const ImVec4& innerColor /* = ImVec4(0, 0, 0, 0)*/) {
  if (ImGui::IsRectVisible(size)) {
    auto cursorPos = ImGui::GetCursorScreenPos();
    auto drawList = ImGui::GetWindowDrawList();
    DrawIcon(drawList, cursorPos, cursorPos + size, type, filled,
             ImColor(color), ImColor(innerColor));
  }

  ImGui::Dummy(size);
}
// vh: norm
void DrawPinIcon(const Pin& pin, bool connected, float alpha) {
  if (pin.Type != PinType::Flow) {
    if (pin.editable) {
      ImGui::Dummy(ImVec2{20, 24});
    } else {
      ImGui::Dummy(ImVec2{24, 24});
    }

    return;
  }

  const auto type = GetIconType(pin.Type);
  const auto size = ImVec2{24, 24};
  const auto color = [&pin, alpha]() {
    auto icon_color = GetIconColor(pin.Type);
    icon_color.Value.w = alpha;
    return icon_color;
  }();

  Icon(size, type, connected, color,
       ImColor{32, 32, 32, static_cast<int>(alpha * 255)});
}
// vh: bad
void DrawPinField(Pin& pin) {
  ImGui::Spring(0);

  if (pin.Type != PinType::Empty) {
    if (pin.editable) {
      ImGui::SetNextItemWidth(100);
      ImGui::InputFloat(pin.Name.c_str(), &pin.value);
    } else {
      ImGui::Text("%.3f %s", pin.value, pin.Name.c_str());
    }
  }

  ImGui::Spring(0);
}
// vh: bad
void DrawBlueprintNodeHeader(const Node& node) {
  ImGui::Spring(0);
  ImGui::TextUnformatted(node.Name.c_str());
  ImGui::Spring(1);
  ImGui::Dummy(ImVec2{0, 28});
  ImGui::Spring(0);
}

void DrawCommentNode(Node& node) {
  const float commentAlpha = 0.75f;

  ImGui::PushStyleVar(ImGuiStyleVar_Alpha, commentAlpha);
  ne::PushStyleColor(ne::StyleColor_NodeBg, ImColor(255, 255, 255, 64));
  ne::PushStyleColor(ne::StyleColor_NodeBorder, ImColor(255, 255, 255, 64));
  ne::BeginNode(node.ID);
  ImGui::PushID(node.ID.AsPointer());
  ImGui::BeginVertical("content");
  ImGui::BeginHorizontal("horizontal");
  ImGui::Spring(1);
  ImGui::TextUnformatted(node.Name.c_str());
  ImGui::Spring(1);
  ImGui::EndHorizontal();
  ne::Group(node.Size);
  ImGui::EndVertical();
  ImGui::PopID();
  ne::EndNode();
  ne::PopStyleColor(2);
  ImGui::PopStyleVar();

  if (ne::BeginGroupHint(node.ID)) {
    // auto alpha   = static_cast<int>(commentAlpha *
    // ImGui::GetStyle().Alpha * 255);
    auto bgAlpha = static_cast<int>(ImGui::GetStyle().Alpha * 255);

    // ImGui::PushStyleVar(ImGuiStyleVar_Alpha, commentAlpha *
    // ImGui::GetStyle().Alpha);

    auto min = ne::GetGroupMin();
    // auto max = ed::GetGroupMax();

    ImGui::SetCursorScreenPos(
        min - ImVec2(-8, ImGui::GetTextLineHeightWithSpacing() + 4));
    ImGui::BeginGroup();
    ImGui::TextUnformatted(node.Name.c_str());
    ImGui::EndGroup();

    auto drawList = ne::GetHintBackgroundDrawList();

    auto hintBounds = GetItemRect();
    auto hintFrameBounds = GetExpandedRect(hintBounds, {.x = 8, .y = 4});

    drawList->AddRectFilled(hintFrameBounds.GetTL(), hintFrameBounds.GetBR(),
                            IM_COL32(255, 255, 255, 64 * bgAlpha / 255), 4.0f);

    drawList->AddRect(hintFrameBounds.GetTL(), hintFrameBounds.GetBR(),
                      IM_COL32(255, 255, 255, 128 * bgAlpha / 255), 4.0f);

    // ImGui::PopStyleVar();
  }
  ne::EndGroupHint();
}
}  // namespace
// vh: norm
App::App(const char* name, int argc, char** argv)
    : Application{name, argc, argv},
      next_object_id_{1},
      left_pane_width{400},
      right_pane_width{800} {}
// vh: ok
auto App::GetNextObjectId() -> int { return next_object_id_++; }
// vh: ok
auto App::GetTextures() -> esc::TexturesHandle& {
  cpp::Expects(textures_.has_value());
  return *textures_;
}
// vh: ok
auto App::GetNodesAndLinks() -> esc::NodesAndLinks& {
  cpp::Expects(nodes_and_links_.has_value());
  return *nodes_and_links_;
}
// vh: norm
auto App::GetNextLinkId() -> ne::LinkId {
  return ne::LinkId{static_cast<uintptr_t>(GetNextObjectId())};
}
// vh: norm
void App::OnStart() {
  cpp::Expects(!editor_context_.has_value());
  cpp::Expects(!textures_.has_value());
  cpp::Expects(!left_pane_.has_value());
  cpp::Expects(!nodes_and_links_.has_value());

  editor_context_.emplace();
  textures_.emplace(shared_from_this());
  left_pane_.emplace(shared_from_this());
  nodes_and_links_.emplace(shared_from_this());

  cpp::Ensures(editor_context_.has_value());
  cpp::Ensures(textures_.has_value());
  cpp::Ensures(left_pane_.has_value());
  cpp::Ensures(nodes_and_links_.has_value());
}
// vh: norm
void App::OnStop() {
  cpp::Expects(left_pane_.has_value());
  cpp::Expects(textures_.has_value());
  cpp::Expects(editor_context_.has_value());
  cpp::Expects(nodes_and_links_.has_value());

  nodes_and_links_.reset();
  left_pane_.reset();
  textures_.reset();
  editor_context_.reset();

  cpp::Ensures(!left_pane_.has_value());
  cpp::Ensures(!textures_.has_value());
  cpp::Ensures(!editor_context_.has_value());
  cpp::Ensures(!nodes_and_links_.has_value());
}
// vh: ok
void App::OnFrame(float /*unused*/) {
  nodes_and_links_->OnFrame();
  DrawFrame();
}
// vh: ok
void App::ShowFlow() {
  cpp::Expects(nodes_and_links_.has_value());

  for (const auto& link : nodes_and_links_->GetLinks()) {
    ne::Flow(link.ID);
  }
}
// vh: bad
void App::DrawContextMenu() {
  const auto open_popup_pos = ImGui::GetMousePos();

  {
    const auto suspend_scope =
        cpp::Scope{[]() { ne::Suspend(); }, []() { ne::Resume(); }};

    if (ne::ShowNodeContextMenu(&context_node_id)) {
      ImGui::OpenPopup("Node Context Menu");
    } else if (ne::ShowPinContextMenu(&context_pin_id)) {
      ImGui::OpenPopup("Pin Context Menu");
    } else if (ne::ShowLinkContextMenu(&context_link_id)) {
      ImGui::OpenPopup("Link Context Menu");
    } else if (ne::ShowBackgroundContextMenu()) {
      ImGui::OpenPopup("Create New Node");
      existing_node_new_link_pin = nullptr;
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

      if (ImGui::BeginPopup("Node Context Menu")) {
        const auto popup_scope = cpp::Scope{[]() { ImGui::EndPopup(); }};

        ImGui::TextUnformatted("Node Context Menu");
        ImGui::Separator();

        const auto* node = nodes_and_links_->FindNode(context_node_id);

        if (node != nullptr) {
          ImGui::Text("ID: %p", node->ID.AsPointer());
          ImGui::Text("Type: %s", (node->Type == NodeType::Blueprint)
                                      ? "Blueprint"
                                      : "Not blueprint");
          ImGui::Text("Inputs: %d", static_cast<int>(node->Inputs.size()));
          ImGui::Text("Outputs: %d", static_cast<int>(node->Outputs.size()));
        } else {
          ImGui::Text("Unknown node: %p", context_node_id.AsPointer());
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Delete")) {
          ne::DeleteNode(context_node_id);
        }
      }

      if (ImGui::BeginPopup("Pin Context Menu")) {
        const auto popup_scope = cpp::Scope{[]() { ImGui::EndPopup(); }};

        const auto* pin = nodes_and_links_->FindPin(context_pin_id);

        ImGui::TextUnformatted("Pin Context Menu");
        ImGui::Separator();

        if (pin != nullptr) {
          ImGui::Text("ID: %p", pin->ID.AsPointer());

          if (pin->node != nullptr) {
            ImGui::Text("Node: %p", pin->node->ID.AsPointer());
          } else {
            ImGui::Text("Node: %s", "<none>");
          }
        } else {
          ImGui::Text("Unknown pin: %p", context_pin_id.AsPointer());
        }
      }

      if (ImGui::BeginPopup("Link Context Menu")) {
        const auto popup_scope = cpp::Scope{[]() { ImGui::EndPopup(); }};

        const auto* link = nodes_and_links_->FindLink(context_link_id);

        ImGui::TextUnformatted("Link Context Menu");
        ImGui::Separator();

        if (link != nullptr) {
          ImGui::Text("ID: %p", link->ID.AsPointer());
          ImGui::Text("From: %p", link->StartPinID.AsPointer());
          ImGui::Text("To: %p", link->EndPinID.AsPointer());
        } else {
          ImGui::Text("Unknown link: %p", context_link_id.AsPointer());
        }

        ImGui::Separator();

        if (ImGui::MenuItem("Delete")) {
          ne::DeleteLink(context_link_id);
        }
      }

      if (ImGui::BeginPopup("Create New Node")) {
        const auto popup_scope = cpp::Scope{[]() { ImGui::EndPopup(); }};

        ImGui::TextUnformatted("Create New Node");
        ImGui::Separator();

        for (const auto& node_type_name :
             esc::NodesAndLinks::GetNodeTypeNames()) {
          if (ImGui::MenuItem(node_type_name.c_str())) {
            auto* new_node =
                nodes_and_links_->SpawnNodeByTypeName(node_type_name);

            if (new_node == nullptr) {
              continue;
            }

            ne::SetNodePosition(new_node->ID, open_popup_pos);

            if (const auto node_created_by_link_from_existing_one =
                    existing_node_new_link_pin != nullptr) {
              nodes_and_links_->SpawnLinkFromPinToNode(
                  existing_node_new_link_pin, new_node);
            }

            create_new_node = false;
            break;
          }
        }
      } else {
        create_new_node = false;
      }
    }
  }
}
// vh: bad
auto App::CalculateAlphaForPin(const Pin& pin) {
  auto alpha = ImGui::GetStyle().Alpha;

  if ((new_link_pin != nullptr) && !CanCreateLink(new_link_pin, &pin) &&
      (&pin != new_link_pin)) {
    alpha = alpha * (48.0F / 255.0F);
  }

  return alpha;
}
// vh: bad
void App::DrawBlueprintNode(Node& node) {
  cpp::Expects(textures_.has_value());
  cpp::Expects(nodes_and_links_.has_value());

  auto* header_background = textures_->GetTextureIds().header_background;
  auto node_builder = util::BlueprintNodeBuilder{
      header_background, GetTextureWidth(header_background),
      GetTextureHeight(header_background)};

  const auto node_builder_scope =
      cpp::Scope{[&node_builder, &node]() { node_builder.Begin(node.ID); },
                 [&node_builder]() { node_builder.End(); }};

  {
    const auto header_scope = cpp::Scope{
        [&node_builder, &node]() { node_builder.Header(node.Color); },
        [&node_builder]() { node_builder.EndHeader(); }};

    DrawBlueprintNodeHeader(node);
  }

  for (auto& input : node.Inputs) {
    const auto input_scope =
        cpp::Scope{[&node_builder, &input]() { node_builder.Input(input.ID); },
                   [&node_builder]() { node_builder.EndInput(); }};

    const auto alpha = CalculateAlphaForPin(input);

    {
      const auto style_var_scope = cpp::Scope{
          [alpha]() { ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha); },
          []() { ImGui::PopStyleVar(); }};

      DrawPinIcon(input, nodes_and_links_->IsPinLinked(input.ID), alpha);
      DrawPinField(input);
    }
  }

  for (auto& output : node.Outputs) {
    const auto alpha = CalculateAlphaForPin(output);

    {
      const auto style_var_scope = cpp::Scope{
          [alpha]() { ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha); },
          []() { ImGui::PopStyleVar(); }};

      {
        const auto input_scope = cpp::Scope{
            [&node_builder, &output]() { node_builder.Output(output.ID); },
            [&node_builder]() { node_builder.EndOutput(); }};

        DrawPinField(output);
        DrawPinIcon(output, nodes_and_links_->IsPinLinked(output.ID), alpha);
      }
    }
  }
}
// vh: norm
void App::DrawLinks() {
  cpp::Expects(nodes_and_links_.has_value());

  for (const auto& link : nodes_and_links_->GetLinks()) {
    ne::Link(link.ID, link.StartPinID, link.EndPinID, link.Color, 2.0F);
  }
}

void App::DrawLinkConnection() {
  if (ne::BeginCreate(ImColor(255, 255, 255), 2.0f)) {
    auto showLabel = [](const char* label, ImColor color) {
      ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
      auto size = ImGui::CalcTextSize(label);

      auto padding = ImGui::GetStyle().FramePadding;
      auto spacing = ImGui::GetStyle().ItemSpacing;

      ImGui::SetCursorPos(ImGui::GetCursorPos() +
                          ImVec2(spacing.x, -spacing.y));

      auto rectMin = ImGui::GetCursorScreenPos() - padding;
      auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

      auto drawList = ImGui::GetWindowDrawList();
      drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
      ImGui::TextUnformatted(label);
    };

    ne::PinId startPinId = 0, endPinId = 0;
    if (ne::QueryNewLink(&startPinId, &endPinId)) {
      auto startPin = nodes_and_links_->FindPin(startPinId);
      auto endPin = nodes_and_links_->FindPin(endPinId);

      new_link_pin = startPin ? startPin : endPin;

      if (startPin->Kind == PinKind::Input) {
        using std::swap;

        swap(startPin, endPin);
        swap(startPinId, endPinId);
      }

      if (startPin && endPin) {
        if (endPin == startPin) {
          ne::RejectNewItem(ImColor(255, 0, 0), 2.0f);
        } else if (endPin->Kind == startPin->Kind) {
          showLabel("x Incompatible Pin Kind", ImColor(45, 32, 32, 180));
          ne::RejectNewItem(ImColor(255, 0, 0), 2.0f);
        } else if (endPin->node == startPin->node) {
          showLabel("x Cannot connect to self", ImColor(45, 32, 32, 180));
          ne::RejectNewItem(ImColor(255, 0, 0), 1.0f);
        } else if (endPin->Type != startPin->Type) {
          showLabel("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
          ne::RejectNewItem(ImColor(255, 128, 128), 1.0f);
        } else {
          showLabel("+ Create Link", ImColor(32, 45, 32, 180));
          if (ne::AcceptNewItem(ImColor(128, 255, 128), 4.0f)) {
            auto link = Link{GetNextLinkId(), startPinId, endPinId};
            link.Color = GetIconColor(startPin->Type);
            nodes_and_links_->SpawnLink(link);
          }
        }
      }
    }

    ne::PinId pinId = 0;
    if (ne::QueryNewNode(&pinId)) {
      new_link_pin = nodes_and_links_->FindPin(pinId);
      if (new_link_pin) showLabel("+ Create Node", ImColor(32, 45, 32, 180));

      if (ne::AcceptNewItem()) {
        create_new_node = true;
        existing_node_new_link_pin = nodes_and_links_->FindPin(pinId);
        new_link_pin = nullptr;
        ne::Suspend();
        ImGui::OpenPopup("Create New Node");
        ne::Resume();
      }
    }
  } else
    new_link_pin = nullptr;

  ne::EndCreate();

  if (ne::BeginDelete()) {
    ne::LinkId linkId = 0;
    while (ne::QueryDeletedLink(&linkId)) {
      if (ne::AcceptDeletedItem()) {
        nodes_and_links_->EraseLinkWithId(linkId);
      }
    }

    ne::NodeId nodeId = 0;
    while (ne::QueryDeletedNode(&nodeId)) {
      if (ne::AcceptDeletedItem()) {
        nodes_and_links_->EraseNodeWithId(nodeId);
      }
    }
  }

  ne::EndDelete();
}
// vh: bad
void App::DrawFrame() {
  cpp::Expects(left_pane_.has_value());
  cpp::Expects(nodes_and_links_.has_value());

  DrawSplitter(4.0F, &left_pane_width, &right_pane_width, 50.0F, 50.0F);
  left_pane_->Draw(left_pane_width - 4.0F);
  ImGui::SameLine(0.0F, 10.0F);

  {
    const auto node_editor_scope =
        cpp::Scope{[]() { ne::Begin("Node editor"); }, []() { ne::End(); }};

    for (auto& node : nodes_and_links_->GetNodes()) {
      if (node.Type == NodeType::Blueprint) {
        DrawBlueprintNode(node);
      }
    }

    for (auto& node : nodes_and_links_->GetNodes()) {
      if (node.Type == NodeType::Comment) {
        DrawCommentNode(node);
      }
    }

    DrawLinks();

    if (!create_new_node) {
      DrawLinkConnection();
    }

    DrawContextMenu();
  }

  ImGui::ShowDemoWindow();
  ImGui::ShowMetricsWindow();
}