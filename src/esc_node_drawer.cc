#include "esc_node_drawer.h"

#include <imgui_internal.h>

#include "cpp_assert.h"
#include "imgui_node_editor.h"

namespace ne = ax::NodeEditor;

namespace esc {
// vh: norm
NodeDrawer::NodeDrawer(ne::NodeId node_id) : node_id_(node_id) {
  ne::PushStyleVar(ne::StyleVar_NodePadding, ImVec4{8, 4, 8, 8});
  ne::BeginNode(node_id);

  ImGui::PushID(node_id.AsPointer());
  SetStage(Stage::kBegin);
}
// vh: norm
NodeDrawer::~NodeDrawer() {
  SetStage(Stage::kEnd);
  ne::EndNode();

  DrawHeader();

  ImGui::PopID();
  ne::PopStyleVar();
  SetStage(Stage::kInvalid);
}
// vh: norm
auto NodeDrawer::AddHeader(const TextureWithDims& header_texture,
                           const ImColor& header_color) -> cpp::ScopeFunction {
  header_ = Header{.texture = header_texture, .color = header_color};

  SetStage(Stage::kHeader);
  return cpp::ScopeFunction{[this]() { SetStage(Stage::kContent); }};
}
// vh: norm
auto NodeDrawer::AddPin(ne::PinId id, ne::PinKind kind) -> cpp::ScopeFunction {
  if (stage_ == Stage::kBegin) {
    SetStage(Stage::kContent);
  }

  const auto next_stage =
      (kind == ne::PinKind::Input) ? Stage::kInput : Stage::kOutput;
  const auto apply_padding = (stage_ == next_stage);

  SetStage(next_stage);

  if (apply_padding) {
    ImGui::Spring(0);
  }

  ne::BeginPin(id, kind);
  ImGui::BeginHorizontal(id.AsPointer());

  return cpp::ScopeFunction{[]() {
    ImGui::EndHorizontal();
    ne::EndPin();
  }};
}
// vh: norm
void NodeDrawer::ProcessOldStage(Stage old_stage) {
  switch (old_stage) {
    case Stage::kHeader:
      ImGui::EndHorizontal();

      cpp::Expects(header_.has_value());
      header_->rect = ImRect{ImGui::GetItemRectMin(), ImGui::GetItemRectMax()};

      ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.y * 2.0f);
      break;

    case Stage::kInput:
    case Stage::kOutput:
      ne::PopStyleVar(2);
      ImGui::Spring(1, 0);
      ImGui::EndVertical();
      break;

    default:
      break;
  }
}
// vh: norm
void NodeDrawer::ProcessNewStage(Stage old_stage) {
  switch (stage_) {
    case Stage::kBegin:
      ImGui::BeginVertical("node");
      break;

    case Stage::kHeader:
      ImGui::BeginHorizontal("header");
      break;

    case Stage::kContent:
      if (old_stage == Stage::kBegin) {
        ImGui::Spring(0);
      }

      ImGui::BeginHorizontal("content");
      ImGui::Spring(0, 0);
      break;

    case Stage::kInput:
      ImGui::BeginVertical("inputs", {0, 0}, 0.0F);
      ne::PushStyleVar(ne::StyleVar_PivotAlignment, {0, 0.5F});
      ne::PushStyleVar(ne::StyleVar_PivotSize, {0, 0});

      if (!header_.has_value()) {
        ImGui::Spring(1, 0);
      }

      break;

    case Stage::kOutput:
      if (old_stage == Stage::kInput) {
        ImGui::Spring(1);
      } else {
        ImGui::Spring(1, 0);
      }

      ImGui::BeginVertical("outputs", {0, 0}, 1.0F);
      ne::PushStyleVar(ne::StyleVar_PivotAlignment, {1.0f, 0.5f});
      ne::PushStyleVar(ne::StyleVar_PivotSize, {0, 0});

      if (!header_.has_value()) {
        ImGui::Spring(1, 0);
      }

      break;

    case Stage::kEnd:
      if (old_stage == Stage::kInput) {
        ImGui::Spring(1, 0);
      }

      if (old_stage != Stage::kBegin) {
        ImGui::EndHorizontal();
      }

      ImGui::EndVertical();
      break;

    default:
      break;
  }
}
// vh: norm
void NodeDrawer::SetStage(Stage next_stage) {
  if (next_stage == stage_) {
    return;
  }

  auto old_stage = stage_;
  stage_ = next_stage;

  ProcessOldStage(old_stage);
  ProcessNewStage(old_stage);
}
// vh: norm
void NodeDrawer::DrawHeader() {
  if (!header_.has_value() || !ImGui::IsItemVisible()) {
    return;
  }

  auto* drawList = ne::GetNodeBackgroundDrawList(node_id_);

  const auto texture_uv =
      ImVec2{header_->rect.GetWidth() /
                 (4.0F * static_cast<float>(header_->texture.width)),
             header_->rect.GetHeight() /
                 (4.0F * static_cast<float>(header_->texture.height))};
  const auto alpha = static_cast<int>(255 * ImGui::GetStyle().Alpha);
  const auto header_color =
      IM_COL32(0, 0, 0, alpha) | (header_->color & IM_COL32(255, 255, 255, 0));
  const auto& style = ne::GetStyle();
  const auto half_border_width = style.NodeBorderWidth * 0.5f;

  drawList->AddImageRounded(
      header_->texture.id,
      header_->rect.Min - ImVec2{8 - half_border_width, 4 - half_border_width},
      header_->rect.Max + ImVec2{8 - half_border_width, 0}, ImVec2{0.0F, 0.0F},
      texture_uv, header_color, style.NodeRounding,
      ImDrawFlags_RoundCornersTop);

  drawList->AddLine(
      header_->rect.GetBL() + ImVec2{(half_border_width - 8), -0.5F},
      header_->rect.GetBR() + ImVec2{(8 - half_border_width), -0.5F},
      ImColor{255, 255, 255, 96 * alpha / (3 * 255)}, 1.0F);
}
}  // namespace esc