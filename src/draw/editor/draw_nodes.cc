#include <cstdint>
#include <memory>
#include <unordered_map>
#include <variant>
#include <vector>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <iostream>
#include <optional>
#include <string_view>

#include "cpp_assert.h"
#include "cpp_scope.h"
#include "draw_flow_icon.h"
#include "draw_nodes.h"
#include "frame_node.h"
#include "imgui.h"
#include "imgui_node_editor.h"

namespace esc::draw {
namespace {
void DrawPinField(const coreui::Pin& pin) {
  const auto label = pin.label;

  ImGui::Spring(0);
  const auto spring_scope = cpp::Scope{[]() { ImGui::Spring(0); }};

  if (std::holds_alternative<std::monostate>(pin.value)) {
    ImGui::TextUnformatted(label.c_str());
    return;
  }

  if (std::holds_alternative<float>(pin.value)) {
    const auto float_value = std::get<float>(pin.value);
    ImGui::Text("%.3f %s", float_value, label.c_str());
    return;
  }

  Expects(std::holds_alternative<float*>(pin.value));

  auto* float_value = std::get<float*>(pin.value);
  ImGui::SetNextItemWidth(100);
  ImGui::InputFloat(label.c_str(), float_value, 0.F, 0.F, "%.3f");
}

auto DrawPinIconArea(const coreui::Pin& pin) -> std::optional<ImRect> {
  const auto pin_id = pin.id;
  auto area_size = ImVec2{24, 24};

  if (!pin_id.has_value()) {
    if (std::holds_alternative<float*>(pin.value)) {
      area_size.x -= 4;
    }

    ImGui::Dummy(area_size);
    return std::nullopt;
  }

  if (!ImGui::IsRectVisible(area_size)) {
    return std::nullopt;
  }

  DrawFlowIcon(area_size, pin.color, pin.filled);
  ImGui::Dummy(area_size);

  return ImRect{ImGui::GetItemRectMin(), ImGui::GetItemRectMax()};
}

auto DrawNode(const coreui::Node& node) {
  auto pin_rects = std::unordered_map<uintptr_t, ImRect>{};
  int layout_id_{};

  const auto node_id_ = node.id;

  ne::PushStyleVar(ne::StyleVar_NodePadding, ImVec4{8, 4, 8, 8});
  ne::BeginNode(node_id_);
  ImGui::PushID(node_id_.AsPointer());
  ImGui::BeginVertical("node");

  struct Header {
    coreui::Texture texture{};
    ImColor color{};
    ImRect rect{};
  };

  std::optional<Header> header_{};

  if (const auto header = node.header) {
    auto color = header->color;

    header_ = Header{.texture = header->texture, .color = color};

    ImGui::BeginHorizontal("header");

    ImGui::Spring(0);
    ImGui::TextUnformatted(header->label.c_str());
    ImGui::Spring(1);
    ImGui::Dummy(ImVec2{0, 28});
    ImGui::Spring(0);

    ImGui::EndHorizontal();

    Expects(header_.has_value());
    header_->rect = ImRect{ImGui::GetItemRectMin(), ImGui::GetItemRectMax()};

    ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.y * 2.0f);
  } else {
    ImGui::Spring(0);
  }

  ImGui::BeginHorizontal("content");
  ImGui::Spring(0, 0);

  if (!node.input_pins.empty()) {
    ImGui::BeginVertical("inputs", {0, 0}, 0.0F);
    ne::PushStyleVar(ne::StyleVar_PivotAlignment, {0, 0.5F});
    ne::PushStyleVar(ne::StyleVar_PivotSize, {0, 0});

    // if (!header_.has_value()) {
    //   ImGui::Spring(1, 0);
    // }

    for (auto& pin_drawer : node.input_pins) {
      const auto pin_kind = ne::PinKind::Input;
      const auto id = pin_drawer.id;
      const auto kind = pin_kind;

      if (id.has_value()) {
        ne::BeginPin(*id, kind);
        ImGui::BeginHorizontal(id->AsPointer());
      } else {
        ImGui::BeginHorizontal(layout_id_++);
      }

      const auto pin_rect = DrawPinIconArea(pin_drawer);

      if (id.has_value() && pin_rect.has_value()) {
        pin_rects.emplace(*id, *pin_rect);
      }

      DrawPinField(pin_drawer);

      ImGui::EndHorizontal();

      if (id.has_value()) {
        ne::EndPin();
      }
    }

    ne::PopStyleVar(2);
    ImGui::Spring(1, 0);
    ImGui::EndVertical();
  }

  if (!node.output_pins.empty()) {
    if (!node.input_pins.empty()) {
      ImGui::Spring(1);
    }

    ImGui::BeginVertical("outputs", {0, 0}, 1.0F);
    ne::PushStyleVar(ne::StyleVar_PivotAlignment, {1.0f, 0.5f});
    ne::PushStyleVar(ne::StyleVar_PivotSize, {0, 0});

    // if (!header_.has_value()) {
    //   ImGui::Spring(1, 0);
    // }

    for (const auto& pin_drawer : node.output_pins) {
      const auto pin_kind = ne::PinKind::Output;
      const auto id = pin_drawer.id;
      const auto kind = pin_kind;

      if (id.has_value()) {
        ne::BeginPin(*id, kind);
        ImGui::BeginHorizontal(id->AsPointer());
      } else {
        ImGui::BeginHorizontal(layout_id_++);
      }

      DrawPinField(pin_drawer);

      const auto pin_rect = DrawPinIconArea(pin_drawer);

      if (id.has_value() && pin_rect.has_value()) {
        pin_rects.emplace(*id, *pin_rect);
      }

      ImGui::EndHorizontal();

      if (id.has_value()) {
        ne::EndPin();
      }
    }

    ne::PopStyleVar(2);
    ImGui::Spring(1, 0);
    ImGui::EndVertical();
  }

  ImGui::EndHorizontal();
  ImGui::EndVertical();

  ne::EndNode();

  if (header_.has_value() && ImGui::IsItemVisible()) {
    auto* drawList = ne::GetNodeBackgroundDrawList(node_id_);

    const auto texture_uv =
        ImVec2{header_->rect.GetWidth() /
                   (4.0F * static_cast<float>(header_->texture.width)),
               header_->rect.GetHeight() /
                   (4.0F * static_cast<float>(header_->texture.height))};
    const auto alpha = static_cast<int>(255 * ImGui::GetStyle().Alpha);
    const auto header_color = IM_COL32(0, 0, 0, alpha) |
                              (header_->color & IM_COL32(255, 255, 255, 0));
    const auto& style = ne::GetStyle();
    const auto half_border_width = style.NodeBorderWidth * 0.5f;

    drawList->AddImageRounded(
        header_->texture.id,
        header_->rect.Min -
            ImVec2{8 - half_border_width, 4 - half_border_width},
        header_->rect.Max + ImVec2{8 - half_border_width, 0},
        ImVec2{0.0F, 0.0F}, texture_uv, header_color, style.NodeRounding,
        ImDrawFlags_RoundCornersTop);

    drawList->AddLine(
        header_->rect.GetBL() + ImVec2{(half_border_width - 8), -0.5F},
        header_->rect.GetBR() + ImVec2{(8 - half_border_width), -0.5F},
        ImColor{255, 255, 255, 96 * alpha / (3 * 255)}, 1.0F);
  }

  ImGui::PopID();
  ne::PopStyleVar();

  return pin_rects;
}
}  // namespace

void DrawNodes(coreui::Nodes& nodes) {
  auto pin_rects = std::unordered_map<uintptr_t, ImRect>{};

  for (const auto& node : nodes.nodes) {
    const auto node_pin_rects = DrawNode(node);

    for (const auto& [pin, rect] : node_pin_rects) {
      pin_rects.emplace(pin, rect);
    }
  }

  nodes.pin_rects = std::move(pin_rects);
}
}  // namespace esc::draw