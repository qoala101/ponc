#include "esc_app.h"

#include <application.h>
#include <imgui_node_editor.h>
#include <sys/types.h>

#include <memory>

#include "esc_builders.h"
#include "esc_cpp.h"
#include "esc_enums.h"
#include "esc_nodes_and_links.h"
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
  } else {
    auto triangleStart = rect_center_x + 0.32f * rect_w;

    auto rect_offset = -static_cast<int>(rect_w * 0.25f * 0.25f);

    rect.Min.x += rect_offset;
    rect.Max.x += rect_offset;
    rect_x += rect_offset;
    rect_center_x += rect_offset * 0.5f;
    rect_center.x += rect_offset * 0.5f;

    if (type == IconType::Circle) {
      const auto c = rect_center;

      if (!filled) {
        const auto r = 0.5f * rect_w / 2.0f - 0.5f;

        if (innerColor & 0xFF000000)
          drawList->AddCircleFilled(c, r, innerColor, 12 + extra_segments);
        drawList->AddCircle(c, r, color, 12 + extra_segments,
                            2.0f * outline_scale);
      } else {
        drawList->AddCircleFilled(c, 0.5f * rect_w / 2.0f, color,
                                  12 + extra_segments);
      }
    }

    if (type == IconType::Square) {
      if (filled) {
        const auto r = 0.5f * rect_w / 2.0f;
        const auto p0 = rect_center - ImVec2(r, r);
        const auto p1 = rect_center + ImVec2(r, r);

        drawList->AddRectFilled(p0, p1, color, 0, ImDrawFlags_RoundCornersAll);
      } else {
        const auto r = 0.5f * rect_w / 2.0f - 0.5f;
        const auto p0 = rect_center - ImVec2(r, r);
        const auto p1 = rect_center + ImVec2(r, r);

        if (innerColor & 0xFF000000) {
          drawList->AddRectFilled(p0, p1, innerColor, 0,
                                  ImDrawFlags_RoundCornersAll);
        }

        drawList->AddRect(p0, p1, color, 0, ImDrawFlags_RoundCornersAll,
                          2.0f * outline_scale);
      }
    }

    if (type == IconType::Grid) {
      const auto r = 0.5f * rect_w / 2.0f;
      const auto w = ceilf(r / 3.0f);

      const auto baseTl = ImVec2(floorf(rect_center_x - w * 2.5f),
                                 floorf(rect_center_y - w * 2.5f));
      const auto baseBr = ImVec2(floorf(baseTl.x + w), floorf(baseTl.y + w));

      auto tl = baseTl;
      auto br = baseBr;
      for (int i = 0; i < 3; ++i) {
        tl.x = baseTl.x;
        br.x = baseBr.x;
        drawList->AddRectFilled(tl, br, color);
        tl.x += w * 2;
        br.x += w * 2;
        if (i != 1 || filled) drawList->AddRectFilled(tl, br, color);
        tl.x += w * 2;
        br.x += w * 2;
        drawList->AddRectFilled(tl, br, color);

        tl.y += w * 2;
        br.y += w * 2;
      }

      triangleStart = br.x + w + 1.0f / 24.0f * rect_w;
    }

    if (type == IconType::RoundSquare) {
      if (filled) {
        const auto r = 0.5f * rect_w / 2.0f;
        const auto cr = r * 0.5f;
        const auto p0 = rect_center - ImVec2(r, r);
        const auto p1 = rect_center + ImVec2(r, r);

        drawList->AddRectFilled(p0, p1, color, cr, ImDrawFlags_RoundCornersAll);
      } else {
        const auto r = 0.5f * rect_w / 2.0f - 0.5f;
        const auto cr = r * 0.5f;
        const auto p0 = rect_center - ImVec2(r, r);
        const auto p1 = rect_center + ImVec2(r, r);

        if (innerColor & 0xFF000000) {
          drawList->AddRectFilled(p0, p1, innerColor, cr,
                                  ImDrawFlags_RoundCornersAll);
        }

        drawList->AddRect(p0, p1, color, cr, ImDrawFlags_RoundCornersAll,
                          2.0f * outline_scale);
      }
    } else if (type == IconType::Diamond) {
      if (filled) {
        const auto r = 0.607f * rect_w / 2.0f;
        const auto c = rect_center;

        drawList->PathLineTo(c + ImVec2(0, -r));
        drawList->PathLineTo(c + ImVec2(r, 0));
        drawList->PathLineTo(c + ImVec2(0, r));
        drawList->PathLineTo(c + ImVec2(-r, 0));
        drawList->PathFillConvex(color);
      } else {
        const auto r = 0.607f * rect_w / 2.0f - 0.5f;
        const auto c = rect_center;

        drawList->PathLineTo(c + ImVec2(0, -r));
        drawList->PathLineTo(c + ImVec2(r, 0));
        drawList->PathLineTo(c + ImVec2(0, r));
        drawList->PathLineTo(c + ImVec2(-r, 0));

        if (innerColor & 0xFF000000)
          drawList->AddConvexPolyFilled(drawList->_Path.Data,
                                        drawList->_Path.Size, innerColor);

        drawList->PathStroke(color, true, 2.0f * outline_scale);
      }
    } else {
      const auto triangleTip = triangleStart + rect_w * (0.45f - 0.32f);

      drawList->AddTriangleFilled(
          ImVec2(ceilf(triangleTip), rect_y + rect_h * 0.5f),
          ImVec2(triangleStart, rect_center_y + 0.15f * rect_h),
          ImVec2(triangleStart, rect_center_y - 0.15f * rect_h), color);
    }
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
}  // namespace
// vh: norm
App::App(const char* name, int argc, char** argv)
    : Application{name, argc, argv}, next_object_id_{1} {}
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
auto App::GetNextLinkId() {
  return ne::LinkId{static_cast<uintptr_t>(GetNextObjectId())};
}
// vh: norm
auto App::CreateEditorConfig() {
  cpp::Expects(nodes_and_links_.has_value());

  auto config = ne::Config{};

  config.SettingsFile = "Blueprints.json";
  config.UserPointer = this;

  config.LoadNodeSettings = [](const auto node_id, auto* data,
                               auto* user_pointer) -> size_t {
    auto* self = static_cast<App*>(user_pointer);
    const auto* node = self->nodes_and_links_->FindNode(node_id);

    if (node == nullptr) {
      return 0;
    }

    if (data != nullptr) {
      strcpy(data, node->State.data());
    }

    return node->State.size();
  };

  config.SaveNodeSettings = [](const auto node_id, const auto* data,
                               const auto size, const auto,
                               auto* user_pointer) -> bool {
    auto* self = static_cast<App*>(user_pointer);
    auto* node = self->nodes_and_links_->FindNode(node_id);

    if (node == nullptr) {
      return false;
    }

    node->State.assign(data, size);

    return true;
  };

  return config;
}
// vh: norm
void App::OnStart() {
  cpp::Expects(!editor_context_.has_value());
  cpp::Expects(!textures_.has_value());
  cpp::Expects(!left_panel_.has_value());
  cpp::Expects(!nodes_and_links_.has_value());

  nodes_and_links_.emplace(shared_from_this());
  editor_context_.emplace(CreateEditorConfig());
  textures_.emplace(shared_from_this());
  left_panel_.emplace(shared_from_this());

  AddInitialNodes();
  AddInitialLinks();

  cpp::Ensures(editor_context_.has_value());
  cpp::Ensures(textures_.has_value());
  cpp::Ensures(left_panel_.has_value());
  cpp::Ensures(nodes_and_links_.has_value());
}
// vh: norm
void App::AddInitialNodes() {
  cpp::Expects(nodes_and_links_.has_value());

  auto* node = static_cast<Node*>(nullptr);

  node = nodes_and_links_->SpawnInputActionNode();
  ne::SetNodePosition(node->ID, ImVec2(-252, 220));
  node = nodes_and_links_->SpawnBranchNode();
  ne::SetNodePosition(node->ID, ImVec2(-300, 351));
  node = nodes_and_links_->SpawnDoNNode();
  ne::SetNodePosition(node->ID, ImVec2(-238, 504));
  node = nodes_and_links_->SpawnOutputActionNode();
  ne::SetNodePosition(node->ID, ImVec2(71, 80));
  node = nodes_and_links_->SpawnSetTimerNode();
  ne::SetNodePosition(node->ID, ImVec2(168, 316));

  node = nodes_and_links_->SpawnTreeSequenceNode();
  ne::SetNodePosition(node->ID, ImVec2(1028, 329));
  node = nodes_and_links_->SpawnTreeTaskNode();
  ne::SetNodePosition(node->ID, ImVec2(1204, 458));
  node = nodes_and_links_->SpawnTreeTask2Node();
  ne::SetNodePosition(node->ID, ImVec2(868, 538));

  node = nodes_and_links_->SpawnComment();
  ne::SetNodePosition(node->ID, ImVec2(112, 576));
  ne::SetGroupSize(node->ID, ImVec2(384, 154));
  node = nodes_and_links_->SpawnComment();
  ne::SetNodePosition(node->ID, ImVec2(800, 224));
  ne::SetGroupSize(node->ID, ImVec2(640, 400));

  node = nodes_and_links_->SpawnLessNode();
  ne::SetNodePosition(node->ID, ImVec2(366, 652));
  node = nodes_and_links_->SpawnWeirdNode();
  ne::SetNodePosition(node->ID, ImVec2(144, 652));
  node = nodes_and_links_->SpawnMessageNode();
  ne::SetNodePosition(node->ID, ImVec2(-348, 698));
  node = nodes_and_links_->SpawnPrintStringNode();
  ne::SetNodePosition(node->ID, ImVec2(-69, 652));

  node = nodes_and_links_->SpawnHoudiniTransformNode();
  ne::SetNodePosition(node->ID, ImVec2(500, -70));
  node = nodes_and_links_->SpawnHoudiniGroupNode();
  ne::SetNodePosition(node->ID, ImVec2(500, 42));

  nodes_and_links_->BuildNodes();
}
// vh: norm
void App::AddInitialLinks() {
  cpp::Expects(nodes_and_links_.has_value());

  nodes_and_links_->SpawnLink({GetNextLinkId(),
                               nodes_and_links_->GetNodes()[5].Outputs[0].ID,
                               nodes_and_links_->GetNodes()[6].Inputs[0].ID});
  nodes_and_links_->SpawnLink({GetNextLinkId(),
                               nodes_and_links_->GetNodes()[5].Outputs[0].ID,
                               nodes_and_links_->GetNodes()[7].Inputs[0].ID});
  nodes_and_links_->SpawnLink({GetNextLinkId(),
                               nodes_and_links_->GetNodes()[14].Outputs[0].ID,
                               nodes_and_links_->GetNodes()[15].Inputs[0].ID});
}
// vh: norm
void App::OnStop() {
  cpp::Expects(left_panel_.has_value());
  cpp::Expects(textures_.has_value());
  cpp::Expects(editor_context_.has_value());
  cpp::Expects(nodes_and_links_.has_value());

  left_panel_.reset();
  textures_.reset();
  editor_context_.reset();
  nodes_and_links_.reset();

  cpp::Ensures(!left_panel_.has_value());
  cpp::Ensures(!textures_.has_value());
  cpp::Ensures(!editor_context_.has_value());
  cpp::Ensures(!nodes_and_links_.has_value());
}
// vh: ok
void App::OnFrame(float /*unused*/) { DrawFrame(); }
// vh: ok
void App::ShowFlow() {
  cpp::Expects(nodes_and_links_.has_value());

  for (const auto& link : nodes_and_links_->GetLinks()) {
    ne::Flow(link.ID);
  }
}

void App::DrawFrame() {
  cpp::Expects(left_panel_.has_value());
  cpp::Expects(textures_.has_value());
  cpp::Expects(nodes_and_links_.has_value());

  static ne::NodeId contextNodeId = 0;
  static ne::LinkId contextLinkId = 0;
  static ne::PinId contextPinId = 0;
  static bool createNewNode = false;
  static Pin* newNodeLinkPin = nullptr;
  static Pin* newLinkPin = nullptr;

  static float leftPaneWidth = 400.0f;
  static float rightPaneWidth = 800.0f;
  DrawSplitter(4.0F, &leftPaneWidth, &rightPaneWidth, 50.0F, 50.0F);

  left_panel_->Draw(leftPaneWidth - 4.0f);

  ImGui::SameLine(0.0f, 12.0f);

  ne::Begin("Node editor");
  {
    auto cursorTopLeft = ImGui::GetCursorScreenPos();

    const auto texture_ids = textures_->GetTextureIds();

    util::BlueprintNodeBuilder builder(
        texture_ids.header_background,
        GetTextureWidth(texture_ids.header_background),
        GetTextureHeight(texture_ids.header_background));

    for (auto& node : nodes_and_links_->GetNodes()) {
      if (node.Type != NodeType::Blueprint && node.Type != NodeType::Simple)
        continue;

      const auto isSimple = node.Type == NodeType::Simple;

      bool hasOutputDelegates = false;
      for (auto& output : node.Outputs)
        if (output.Type == PinType::Delegate) hasOutputDelegates = true;

      builder.Begin(node.ID);
      if (!isSimple) {
        builder.Header(node.Color);
        ImGui::Spring(0);
        ImGui::TextUnformatted(node.Name.c_str());
        ImGui::Spring(1);
        ImGui::Dummy(ImVec2(0, 28));
        if (hasOutputDelegates) {
          ImGui::BeginVertical("delegates", ImVec2(0, 28));
          ImGui::Spring(1, 0);
          for (auto& output : node.Outputs) {
            if (output.Type != PinType::Delegate) continue;

            auto alpha = ImGui::GetStyle().Alpha;
            if (newLinkPin && !CanCreateLink(newLinkPin, &output) &&
                &output != newLinkPin)
              alpha = alpha * (48.0f / 255.0f);

            ne::BeginPin(output.ID, ne::PinKind::Output);
            ne::PinPivotAlignment(ImVec2(1.0f, 0.5f));
            ne::PinPivotSize(ImVec2(0, 0));
            ImGui::BeginHorizontal(output.ID.AsPointer());
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
            if (!output.Name.empty()) {
              ImGui::TextUnformatted(output.Name.c_str());
              ImGui::Spring(0);
            }
            DrawPinIcon(output, nodes_and_links_->IsPinLinked(output.ID),
                        alpha);
            ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.x / 2);
            ImGui::EndHorizontal();
            ImGui::PopStyleVar();
            ne::EndPin();

            // DrawItemRect(ImColor(255, 0, 0));
          }
          ImGui::Spring(1, 0);
          ImGui::EndVertical();
          ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.x / 2);
        } else
          ImGui::Spring(0);
        builder.EndHeader();
      }

      for (auto& input : node.Inputs) {
        auto alpha = ImGui::GetStyle().Alpha;
        if (newLinkPin && !CanCreateLink(newLinkPin, &input) &&
            &input != newLinkPin)
          alpha = alpha * (48.0f / 255.0f);

        builder.Input(input.ID);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
        DrawPinIcon(input, nodes_and_links_->IsPinLinked(input.ID), alpha);
        ImGui::Spring(0);
        if (!input.Name.empty()) {
          ImGui::TextUnformatted(input.Name.c_str());
          ImGui::Spring(0);
        }
        if (input.Type == PinType::Bool) {
          ImGui::Button("Hello");
          ImGui::Spring(0);
        }
        ImGui::PopStyleVar();
        builder.EndInput();
      }

      if (isSimple) {
        builder.Middle();

        ImGui::Spring(1, 0);
        ImGui::TextUnformatted(node.Name.c_str());
        ImGui::Spring(1, 0);
      }

      for (auto& output : node.Outputs) {
        if (!isSimple && output.Type == PinType::Delegate) continue;

        auto alpha = ImGui::GetStyle().Alpha;
        if (newLinkPin && !CanCreateLink(newLinkPin, &output) &&
            &output != newLinkPin)
          alpha = alpha * (48.0f / 255.0f);

        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
        builder.Output(output.ID);
        if (output.Type == PinType::String) {
          static char buffer[128] = "Edit Me\nMultiline!";
          static bool wasActive = false;

          ImGui::PushItemWidth(100.0f);
          ImGui::InputText("##edit", buffer, 127);
          ImGui::PopItemWidth();
          if (ImGui::IsItemActive() && !wasActive) {
            ne::EnableShortcuts(false);
            wasActive = true;
          } else if (!ImGui::IsItemActive() && wasActive) {
            ne::EnableShortcuts(true);
            wasActive = false;
          }
          ImGui::Spring(0);
        }
        if (!output.Name.empty()) {
          ImGui::Spring(0);
          ImGui::TextUnformatted(output.Name.c_str());
        }
        ImGui::Spring(0);
        DrawPinIcon(output, nodes_and_links_->IsPinLinked(output.ID), alpha);
        ImGui::PopStyleVar();
        builder.EndOutput();
      }

      builder.End();
    }

    for (auto& node : nodes_and_links_->GetNodes()) {
      if (node.Type != NodeType::Tree) continue;

      const float rounding = 5.0f;
      const float padding = 12.0f;

      const auto pinBackground = ne::GetStyle().Colors[ne::StyleColor_NodeBg];

      ne::PushStyleColor(ne::StyleColor_NodeBg, ImColor(128, 128, 128, 200));
      ne::PushStyleColor(ne::StyleColor_NodeBorder, ImColor(32, 32, 32, 200));
      ne::PushStyleColor(ne::StyleColor_PinRect, ImColor(60, 180, 255, 150));
      ne::PushStyleColor(ne::StyleColor_PinRectBorder,
                         ImColor(60, 180, 255, 150));

      ne::PushStyleVar(ne::StyleVar_NodePadding, ImVec4(0, 0, 0, 0));
      ne::PushStyleVar(ne::StyleVar_NodeRounding, rounding);
      ne::PushStyleVar(ne::StyleVar_SourceDirection, ImVec2(0.0f, 1.0f));
      ne::PushStyleVar(ne::StyleVar_TargetDirection, ImVec2(0.0f, -1.0f));
      ne::PushStyleVar(ne::StyleVar_LinkStrength, 0.0f);
      ne::PushStyleVar(ne::StyleVar_PinBorderWidth, 1.0f);
      ne::PushStyleVar(ne::StyleVar_PinRadius, 5.0f);
      ne::BeginNode(node.ID);

      ImGui::BeginVertical(node.ID.AsPointer());
      ImGui::BeginHorizontal("inputs");
      ImGui::Spring(0, padding * 2);

      ImRect inputsRect;
      int inputAlpha = 200;
      if (!node.Inputs.empty()) {
        auto& pin = node.Inputs[0];
        ImGui::Dummy(ImVec2(0, padding));
        ImGui::Spring(1, 0);
        inputsRect = GetItemRect();

        ne::PushStyleVar(ne::StyleVar_PinArrowSize, 10.0f);
        ne::PushStyleVar(ne::StyleVar_PinArrowWidth, 10.0f);

        ne::PushStyleVar(ne::StyleVar_PinCorners,
                         ImDrawFlags_RoundCornersBottom);

        ne::BeginPin(pin.ID, ne::PinKind::Input);
        ne::PinPivotRect(inputsRect.GetTL(), inputsRect.GetBR());
        ne::PinRect(inputsRect.GetTL(), inputsRect.GetBR());
        ne::EndPin();
        ne::PopStyleVar(3);

        if (newLinkPin && !CanCreateLink(newLinkPin, &pin) &&
            &pin != newLinkPin)
          inputAlpha = (int)(255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
      } else
        ImGui::Dummy(ImVec2(0, padding));

      ImGui::Spring(0, padding * 2);
      ImGui::EndHorizontal();

      ImGui::BeginHorizontal("content_frame");
      ImGui::Spring(1, padding);

      ImGui::BeginVertical("content", ImVec2(0.0f, 0.0f));
      ImGui::Dummy(ImVec2(160, 0));
      ImGui::Spring(1);
      ImGui::TextUnformatted(node.Name.c_str());
      ImGui::Spring(1);
      ImGui::EndVertical();
      auto contentRect = GetItemRect();

      ImGui::Spring(1, padding);
      ImGui::EndHorizontal();

      ImGui::BeginHorizontal("outputs");
      ImGui::Spring(0, padding * 2);

      ImRect outputsRect;
      int outputAlpha = 200;
      if (!node.Outputs.empty()) {
        auto& pin = node.Outputs[0];
        ImGui::Dummy(ImVec2(0, padding));
        ImGui::Spring(1, 0);
        outputsRect = GetItemRect();

        ne::PushStyleVar(ne::StyleVar_PinCorners, ImDrawFlags_RoundCornersTop);

        ne::BeginPin(pin.ID, ne::PinKind::Output);
        ne::PinPivotRect(outputsRect.GetTL(), outputsRect.GetBR());
        ne::PinRect(outputsRect.GetTL(), outputsRect.GetBR());
        ne::EndPin();
        ne::PopStyleVar();

        if (newLinkPin && !CanCreateLink(newLinkPin, &pin) &&
            &pin != newLinkPin)
          outputAlpha = (int)(255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
      } else
        ImGui::Dummy(ImVec2(0, padding));

      ImGui::Spring(0, padding * 2);
      ImGui::EndHorizontal();

      ImGui::EndVertical();

      ne::EndNode();
      ne::PopStyleVar(7);
      ne::PopStyleColor(4);

      auto drawList = ne::GetNodeBackgroundDrawList(node.ID);

      // const auto fringeScale = ImGui::GetStyle().AntiAliasFringeScale;
      // const auto unitSize    = 1.0f / fringeScale;

      // const auto ImDrawList_AddRect = [](ImDrawList* drawList, const
      // ImVec2& a, const ImVec2& b, ImU32 col, float rounding, int
      // rounding_corners, float thickness)
      //{
      //     if ((col >> 24) == 0)
      //         return;
      //     drawList->PathRect(a, b, rounding, rounding_corners);
      //     drawList->PathStroke(col, true, thickness);
      // };

      const auto topRoundCornersFlags = ImDrawFlags_RoundCornersTop;
      const auto bottomRoundCornersFlags = ImDrawFlags_RoundCornersBottom;

      drawList->AddRectFilled(
          inputsRect.GetTL() + ImVec2(0, 1), inputsRect.GetBR(),
          IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y),
                   (int)(255 * pinBackground.z), inputAlpha),
          4.0f, bottomRoundCornersFlags);
      // ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
      drawList->AddRect(
          inputsRect.GetTL() + ImVec2(0, 1), inputsRect.GetBR(),
          IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y),
                   (int)(255 * pinBackground.z), inputAlpha),
          4.0f, bottomRoundCornersFlags);
      // ImGui::PopStyleVar();
      drawList->AddRectFilled(
          outputsRect.GetTL(), outputsRect.GetBR() - ImVec2(0, 1),
          IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y),
                   (int)(255 * pinBackground.z), outputAlpha),
          4.0f, topRoundCornersFlags);
      // ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
      drawList->AddRect(
          outputsRect.GetTL(), outputsRect.GetBR() - ImVec2(0, 1),
          IM_COL32((int)(255 * pinBackground.x), (int)(255 * pinBackground.y),
                   (int)(255 * pinBackground.z), outputAlpha),
          4.0f, topRoundCornersFlags);
      // ImGui::PopStyleVar();
      drawList->AddRectFilled(contentRect.GetTL(), contentRect.GetBR(),
                              IM_COL32(24, 64, 128, 200), 0.0f);
      // ImGui::PushStyleVar(ImGuiStyleVar_AntiAliasFringeScale, 1.0f);
      drawList->AddRect(contentRect.GetTL(), contentRect.GetBR(),
                        IM_COL32(48, 128, 255, 100), 0.0f);
      // ImGui::PopStyleVar();
    }

    for (auto& node : nodes_and_links_->GetNodes()) {
      if (node.Type != NodeType::Houdini) continue;

      const float rounding = 10.0f;
      const float padding = 12.0f;

      ne::PushStyleColor(ne::StyleColor_NodeBg, ImColor(229, 229, 229, 200));
      ne::PushStyleColor(ne::StyleColor_NodeBorder,
                         ImColor(125, 125, 125, 200));
      ne::PushStyleColor(ne::StyleColor_PinRect, ImColor(229, 229, 229, 60));
      ne::PushStyleColor(ne::StyleColor_PinRectBorder,
                         ImColor(125, 125, 125, 60));

      const auto pinBackground = ne::GetStyle().Colors[ne::StyleColor_NodeBg];

      ne::PushStyleVar(ne::StyleVar_NodePadding, ImVec4(0, 0, 0, 0));
      ne::PushStyleVar(ne::StyleVar_NodeRounding, rounding);
      ne::PushStyleVar(ne::StyleVar_SourceDirection, ImVec2(0.0f, 1.0f));
      ne::PushStyleVar(ne::StyleVar_TargetDirection, ImVec2(0.0f, -1.0f));
      ne::PushStyleVar(ne::StyleVar_LinkStrength, 0.0f);
      ne::PushStyleVar(ne::StyleVar_PinBorderWidth, 1.0f);
      ne::PushStyleVar(ne::StyleVar_PinRadius, 6.0f);
      ne::BeginNode(node.ID);

      ImGui::BeginVertical(node.ID.AsPointer());
      if (!node.Inputs.empty()) {
        ImGui::BeginHorizontal("inputs");
        ImGui::Spring(1, 0);

        ImRect inputsRect;
        int inputAlpha = 200;
        for (auto& pin : node.Inputs) {
          ImGui::Dummy(ImVec2(padding, padding));
          inputsRect = GetItemRect();
          ImGui::Spring(1, 0);
          inputsRect.Min.y -= padding;
          inputsRect.Max.y -= padding;

          const auto allRoundCornersFlags = ImDrawFlags_RoundCornersAll;

          // ed::PushStyleVar(ed::StyleVar_PinArrowSize, 10.0f);
          // ed::PushStyleVar(ed::StyleVar_PinArrowWidth, 10.0f);
          ne::PushStyleVar(ne::StyleVar_PinCorners, allRoundCornersFlags);

          ne::BeginPin(pin.ID, ne::PinKind::Input);
          ne::PinPivotRect(inputsRect.GetCenter(), inputsRect.GetCenter());
          ne::PinRect(inputsRect.GetTL(), inputsRect.GetBR());
          ne::EndPin();
          // ed::PopStyleVar(3);
          ne::PopStyleVar(1);

          auto drawList = ImGui::GetWindowDrawList();
          drawList->AddRectFilled(
              inputsRect.GetTL(), inputsRect.GetBR(),
              IM_COL32((int)(255 * pinBackground.x),
                       (int)(255 * pinBackground.y),
                       (int)(255 * pinBackground.z), inputAlpha),
              4.0f, allRoundCornersFlags);
          drawList->AddRect(inputsRect.GetTL(), inputsRect.GetBR(),
                            IM_COL32((int)(255 * pinBackground.x),
                                     (int)(255 * pinBackground.y),
                                     (int)(255 * pinBackground.z), inputAlpha),
                            4.0f, allRoundCornersFlags);

          if (newLinkPin && !CanCreateLink(newLinkPin, &pin) &&
              &pin != newLinkPin)
            inputAlpha =
                (int)(255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
        }

        // ImGui::Spring(1, 0);
        ImGui::EndHorizontal();
      }

      ImGui::BeginHorizontal("content_frame");
      ImGui::Spring(1, padding);

      ImGui::BeginVertical("content", ImVec2(0.0f, 0.0f));
      ImGui::Dummy(ImVec2(160, 0));
      ImGui::Spring(1);
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.0f, 0.0f, 1.0f));
      ImGui::TextUnformatted(node.Name.c_str());
      ImGui::PopStyleColor();
      ImGui::Spring(1);
      ImGui::EndVertical();
      auto contentRect = GetItemRect();

      ImGui::Spring(1, padding);
      ImGui::EndHorizontal();

      if (!node.Outputs.empty()) {
        ImGui::BeginHorizontal("outputs");
        ImGui::Spring(1, 0);

        ImRect outputsRect;
        int outputAlpha = 200;
        for (auto& pin : node.Outputs) {
          ImGui::Dummy(ImVec2(padding, padding));
          outputsRect = GetItemRect();
          ImGui::Spring(1, 0);
          outputsRect.Min.y += padding;
          outputsRect.Max.y += padding;

          const auto allRoundCornersFlags = ImDrawFlags_RoundCornersAll;
          const auto topRoundCornersFlags = ImDrawFlags_RoundCornersTop;

          ne::PushStyleVar(ne::StyleVar_PinCorners, topRoundCornersFlags);
          ne::BeginPin(pin.ID, ne::PinKind::Output);
          ne::PinPivotRect(outputsRect.GetCenter(), outputsRect.GetCenter());
          ne::PinRect(outputsRect.GetTL(), outputsRect.GetBR());
          ne::EndPin();
          ne::PopStyleVar();

          auto drawList = ImGui::GetWindowDrawList();
          drawList->AddRectFilled(
              outputsRect.GetTL(), outputsRect.GetBR(),
              IM_COL32((int)(255 * pinBackground.x),
                       (int)(255 * pinBackground.y),
                       (int)(255 * pinBackground.z), outputAlpha),
              4.0f, allRoundCornersFlags);
          drawList->AddRect(outputsRect.GetTL(), outputsRect.GetBR(),
                            IM_COL32((int)(255 * pinBackground.x),
                                     (int)(255 * pinBackground.y),
                                     (int)(255 * pinBackground.z), outputAlpha),
                            4.0f, allRoundCornersFlags);

          if (newLinkPin && !CanCreateLink(newLinkPin, &pin) &&
              &pin != newLinkPin)
            outputAlpha =
                (int)(255 * ImGui::GetStyle().Alpha * (48.0f / 255.0f));
        }

        ImGui::EndHorizontal();
      }

      ImGui::EndVertical();

      ne::EndNode();
      ne::PopStyleVar(7);
      ne::PopStyleColor(4);
    }

    for (auto& node : nodes_and_links_->GetNodes()) {
      if (node.Type != NodeType::Comment) continue;

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

        drawList->AddRectFilled(
            hintFrameBounds.GetTL(), hintFrameBounds.GetBR(),
            IM_COL32(255, 255, 255, 64 * bgAlpha / 255), 4.0f);

        drawList->AddRect(hintFrameBounds.GetTL(), hintFrameBounds.GetBR(),
                          IM_COL32(255, 255, 255, 128 * bgAlpha / 255), 4.0f);

        // ImGui::PopStyleVar();
      }
      ne::EndGroupHint();
    }

    for (auto& link : nodes_and_links_->GetLinks())
      ne::Link(link.ID, link.StartPinID, link.EndPinID, link.Color, 2.0f);

    if (!createNewNode) {
      if (ne::BeginCreate(ImColor(255, 255, 255), 2.0f)) {
        auto showLabel = [](const char* label, ImColor color) {
          ImGui::SetCursorPosY(ImGui::GetCursorPosY() -
                               ImGui::GetTextLineHeight());
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

          newLinkPin = startPin ? startPin : endPin;

          if (startPin->Kind == PinKind::Input) {
            std::swap(startPin, endPin);
            std::swap(startPinId, endPinId);
          }

          if (startPin && endPin) {
            if (endPin == startPin) {
              ne::RejectNewItem(ImColor(255, 0, 0), 2.0f);
            } else if (endPin->Kind == startPin->Kind) {
              showLabel("x Incompatible Pin Kind", ImColor(45, 32, 32, 180));
              ne::RejectNewItem(ImColor(255, 0, 0), 2.0f);
            }
            // else if (endPin->Node == startPin->Node)
            //{
            //     showLabel("x Cannot connect to self", ImColor(45, 32, 32,
            //     180)); ed::RejectNewItem(ImColor(255, 0, 0), 1.0f);
            // }
            else if (endPin->Type != startPin->Type) {
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
          newLinkPin = nodes_and_links_->FindPin(pinId);
          if (newLinkPin) showLabel("+ Create Node", ImColor(32, 45, 32, 180));

          if (ne::AcceptNewItem()) {
            createNewNode = true;
            newNodeLinkPin = nodes_and_links_->FindPin(pinId);
            newLinkPin = nullptr;
            ne::Suspend();
            ImGui::OpenPopup("Create New Node");
            ne::Resume();
          }
        }
      } else
        newLinkPin = nullptr;

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
            auto id = std::find_if(
                nodes_and_links_->GetNodes().begin(),
                nodes_and_links_->GetNodes().end(),
                [nodeId](auto& node) { return node.ID == nodeId; });
            if (id != nodes_and_links_->GetNodes().end())
              nodes_and_links_->GetNodes().erase(id);
          }
        }
      }
      ne::EndDelete();
    }

    ImGui::SetCursorScreenPos(cursorTopLeft);
  }

  auto openPopupPosition = ImGui::GetMousePos();
  ne::Suspend();
  if (ne::ShowNodeContextMenu(&contextNodeId))
    ImGui::OpenPopup("Node Context Menu");
  else if (ne::ShowPinContextMenu(&contextPinId))
    ImGui::OpenPopup("Pin Context Menu");
  else if (ne::ShowLinkContextMenu(&contextLinkId))
    ImGui::OpenPopup("Link Context Menu");
  else if (ne::ShowBackgroundContextMenu()) {
    ImGui::OpenPopup("Create New Node");
    newNodeLinkPin = nullptr;
  }
  ne::Resume();

  ne::Suspend();
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
  if (ImGui::BeginPopup("Node Context Menu")) {
    auto node = nodes_and_links_->FindNode(contextNodeId);

    ImGui::TextUnformatted("Node Context Menu");
    ImGui::Separator();
    if (node) {
      ImGui::Text("ID: %p", node->ID.AsPointer());
      ImGui::Text("Type: %s",
                  node->Type == NodeType::Blueprint
                      ? "Blueprint"
                      : (node->Type == NodeType::Tree ? "Tree" : "Comment"));
      ImGui::Text("Inputs: %d", (int)node->Inputs.size());
      ImGui::Text("Outputs: %d", (int)node->Outputs.size());
    } else
      ImGui::Text("Unknown node: %p", contextNodeId.AsPointer());
    ImGui::Separator();
    if (ImGui::MenuItem("Delete")) ne::DeleteNode(contextNodeId);
    ImGui::EndPopup();
  }

  if (ImGui::BeginPopup("Pin Context Menu")) {
    auto pin = nodes_and_links_->FindPin(contextPinId);

    ImGui::TextUnformatted("Pin Context Menu");
    ImGui::Separator();
    if (pin) {
      ImGui::Text("ID: %p", pin->ID.AsPointer());
      if (pin->Node)
        ImGui::Text("Node: %p", pin->Node->ID.AsPointer());
      else
        ImGui::Text("Node: %s", "<none>");
    } else
      ImGui::Text("Unknown pin: %p", contextPinId.AsPointer());

    ImGui::EndPopup();
  }

  if (ImGui::BeginPopup("Link Context Menu")) {
    auto link = nodes_and_links_->FindLink(contextLinkId);

    ImGui::TextUnformatted("Link Context Menu");
    ImGui::Separator();
    if (link) {
      ImGui::Text("ID: %p", link->ID.AsPointer());
      ImGui::Text("From: %p", link->StartPinID.AsPointer());
      ImGui::Text("To: %p", link->EndPinID.AsPointer());
    } else
      ImGui::Text("Unknown link: %p", contextLinkId.AsPointer());
    ImGui::Separator();
    if (ImGui::MenuItem("Delete")) ne::DeleteLink(contextLinkId);
    ImGui::EndPopup();
  }

  if (ImGui::BeginPopup("Create New Node")) {
    auto newNodePostion = openPopupPosition;
    // ImGui::SetCursorScreenPos(ImGui::GetMousePosOnOpeningCurrentPopup());

    // auto drawList = ImGui::GetWindowDrawList();
    // drawList->AddCircleFilled(ImGui::GetMousePosOnOpeningCurrentPopup(), 10.0f,
    // 0xFFFF00FF);

    Node* node = nullptr;
    if (ImGui::MenuItem("Input Action"))
      node = nodes_and_links_->SpawnInputActionNode();
    if (ImGui::MenuItem("Output Action"))
      node = nodes_and_links_->SpawnOutputActionNode();
    if (ImGui::MenuItem("Branch")) node = nodes_and_links_->SpawnBranchNode();
    if (ImGui::MenuItem("Do N")) node = nodes_and_links_->SpawnDoNNode();
    if (ImGui::MenuItem("Set Timer"))
      node = nodes_and_links_->SpawnSetTimerNode();
    if (ImGui::MenuItem("Less")) node = nodes_and_links_->SpawnLessNode();
    if (ImGui::MenuItem("Weird")) node = nodes_and_links_->SpawnWeirdNode();
    if (ImGui::MenuItem("Trace by Channel"))
      node = nodes_and_links_->SpawnTraceByChannelNode();
    if (ImGui::MenuItem("Print String"))
      node = nodes_and_links_->SpawnPrintStringNode();
    ImGui::Separator();
    if (ImGui::MenuItem("Comment")) node = nodes_and_links_->SpawnComment();
    ImGui::Separator();
    if (ImGui::MenuItem("Sequence"))
      node = nodes_and_links_->SpawnTreeSequenceNode();
    if (ImGui::MenuItem("Move To"))
      node = nodes_and_links_->SpawnTreeTaskNode();
    if (ImGui::MenuItem("Random Wait"))
      node = nodes_and_links_->SpawnTreeTask2Node();
    ImGui::Separator();
    if (ImGui::MenuItem("Message")) node = nodes_and_links_->SpawnMessageNode();
    ImGui::Separator();
    if (ImGui::MenuItem("Transform"))
      node = nodes_and_links_->SpawnHoudiniTransformNode();
    if (ImGui::MenuItem("Group"))
      node = nodes_and_links_->SpawnHoudiniGroupNode();

    if (node) {
      nodes_and_links_->BuildNodes();

      createNewNode = false;

      ne::SetNodePosition(node->ID, newNodePostion);

      if (auto startPin = newNodeLinkPin) {
        auto& pins =
            startPin->Kind == PinKind::Input ? node->Outputs : node->Inputs;

        for (auto& pin : pins) {
          if (CanCreateLink(startPin, &pin)) {
            auto endPin = &pin;
            if (startPin->Kind == PinKind::Input) std::swap(startPin, endPin);

            auto link = Link{GetNextLinkId(), startPin->ID, endPin->ID};
            link.Color = GetIconColor(startPin->Type);

            nodes_and_links_->SpawnLink(link);

            break;
          }
        }
      }
    }

    ImGui::EndPopup();
  } else
    createNewNode = false;
  ImGui::PopStyleVar();
  ne::Resume();

  ne::End();

  auto editorMin = ImGui::GetItemRectMin();
  auto editorMax = ImGui::GetItemRectMax();

  ImGui::ShowDemoWindow();
  ImGui::ShowMetricsWindow();
}