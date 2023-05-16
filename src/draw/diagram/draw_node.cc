#include <limits>

#include "draw_colored_text.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

#include <cstdint>
#include <iostream>
#include <memory>
#include <optional>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

#include "cpp_assert.h"
#include "cpp_scope.h"
#include "draw_flow_icon.h"
#include "draw_node.h"
#include "imgui.h"
#include "imgui_node_editor.h"

namespace vh::ponc::draw {
namespace {
auto GetPinTipPos(const ImRect& pin_rect, ne::PinKind pin_kind) {
  return ImVec2{
      (pin_kind == ne::PinKind::Input) ? pin_rect.Min.x : pin_rect.Max.x,
      (pin_rect.Min.y + pin_rect.Max.y) / 2};
}

void DrawPinField(const coreui::Pin& pin) {
  const auto label = pin.label;
  const auto has_label = label.has_value() && !label->text.empty();
  const auto has_value = !std::holds_alternative<std::monostate>(pin.value);

  if (!has_value && !has_label) {
    return;
  }

  ImGui::Spring(0);
  const auto spring_scope = cpp::Scope{[]() { ImGui::Spring(0); }};

  if (!has_value) {
    return;
  }

  if (std::holds_alternative<float>(pin.value)) {
    const auto float_value = std::get<float>(pin.value);
    ImGui::Text("%.2f", float_value);
    return;
  }

  Expects(std::holds_alternative<float*>(pin.value));

  auto* float_value = std::get<float*>(pin.value);
  ImGui::SetNextItemWidth(75);
  ImGui::InputFloat("", float_value, 0, 0, "%.2f");
}

auto DrawPinIconArea(const coreui::Pin& pin, ne::PinKind pin_kind)
    -> std::optional<ImVec2> {
  const auto pin_id = pin.flow_data;
  auto area_size = ImVec2{24, 24};

  if (!pin_id.has_value()) {
    if (std::holds_alternative<float*>(pin.value)) {
      area_size.x -= 4;
    }

    ImGui::Dummy(area_size);
    return std::nullopt;
  }

  DrawFlowIcon(area_size, pin.flow_data->color, pin.flow_data->filled);
  ImGui::Dummy(area_size);

  return GetPinTipPos(ImRect{ImGui::GetItemRectMin(), ImGui::GetItemRectMax()},
                      pin_kind);
}
}  // namespace

void DrawNode(coreui::Node& node, coreui::NodeMover& node_mover) {
  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4);

  auto& core_node = node.GetNode();
  const auto& node_data = node.GetData();
  const auto node_id = core_node.GetId();

  core_node.SetPos(ne::GetNodePosition(node_id));

  ne::PushStyleVar(ne::StyleVar_NodePadding, ImVec4{8, 4, 8, 8});
  ne::BeginNode(node_id);

  ImGui::PushID(node_id.AsPointer());
  ImGui::BeginVertical("Node");

  auto header_rect = std::optional<ImRect>{};

  if (const auto header = node_data.header) {
    ImGui::BeginHorizontal("Header");

    ImGui::Spring(0);
    ImGui::TextUnformatted(header->label.c_str());
    ImGui::Spring(1);
    ImGui::Dummy(ImVec2{0, 28});
    ImGui::Spring(0);

    ImGui::EndHorizontal();

    header_rect = {ImGui::GetItemRectMin(), ImGui::GetItemRectMax()};

    ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.y * 2);
  } else {
    ImGui::Spring(0);
  }

  ImGui::BeginHorizontal("Pins");
  ImGui::Spring(0, 0);

  auto layout_id = 0;
  const auto frame_padding = ImGui::GetStyle().FramePadding;

  if (!node_data.input_pins.empty()) {
    ImGui::BeginVertical("InputPins", {0, 0}, 0);

    // if (!header_.has_value()) {
    //   ImGui::Spring(1, 0);
    // }

    ne::PushStyleVar(ne::StyleVar_PivotAlignment, {0, 0.5F});
    ne::PushStyleVar(ne::StyleVar_PivotSize, {0, 0});

    for (const auto& pin : node_data.input_pins) {
      if (pin.flow_data.has_value()) {
        ImGui::BeginHorizontal(pin.flow_data->id.AsPointer());
        ne::BeginPin(pin.flow_data->id, ne::PinKind::Input);
      } else {
        ImGui::BeginHorizontal(layout_id++);
      }

      node_mover.SetPinPos(pin.flow_data->id,
                           *DrawPinIconArea(pin, ne::PinKind::Input));
      DrawPinField(pin);

      if (pin.flow_data.has_value()) {
        ne::EndPin();
      }

      if (const auto& label = pin.label) {
        auto pos = ImGui::GetCursorPos();
        pos.y += frame_padding.y;
        auto color = label->color;
        color.Value.w = 0.75;
        DrawColoredText(label->text, color, pos);
        ImGui::Spring(0);
      }

      ImGui::EndHorizontal();
    }

    ne::PopStyleVar();
    ne::PopStyleVar();

    ImGui::Spring(1, 0);
    ImGui::EndVertical();
  }

  if (!node_data.output_pins.empty()) {
    if (!node_data.input_pins.empty()) {
      ImGui::Spring(1);
    }

    ImGui::BeginVertical("OutputPins", {0, 0}, 1);

    // if (!header_.has_value()) {
    //   ImGui::Spring(1, 0);
    // }

    ne::PushStyleVar(ne::StyleVar_PivotAlignment, {1, 0.5F});
    ne::PushStyleVar(ne::StyleVar_PivotSize, {0, 0});

    for (auto& pin : node_data.output_pins) {
      if (pin.flow_data.has_value()) {
        ImGui::BeginHorizontal(pin.flow_data->id.AsPointer());
      } else {
        ImGui::BeginHorizontal(layout_id++);
      }

      if (const auto& label = pin.label) {
        ImGui::Spring(0);
        auto pos = ImGui::GetCursorPos();
        pos.y += frame_padding.y;
        DrawColoredText(label->text, label->color, pos);
        pos = ImGui::GetCursorPos();
        pos.y -= frame_padding.y;
        ImGui::SetCursorPos(pos);
      }

      if (pin.flow_data.has_value()) {
        ne::BeginPin(pin.flow_data->id, ne::PinKind::Output);
      }

      DrawPinField(pin);

      const auto label = pin.label;
      const auto has_label = label.has_value() && !label->text.empty();
      const auto has_value = !std::holds_alternative<std::monostate>(pin.value);

      if (!has_value && !has_label) {
        ImGui::Spring(0);
      } else {
        ImGui::Spring(1, 0);
      }

      node_mover.SetPinPos(pin.flow_data->id,
                           *DrawPinIconArea(pin, ne::PinKind::Output));

      if (pin.flow_data.has_value()) {
        ne::EndPin();
      }

      ImGui::EndHorizontal();
    }

    ne::PopStyleVar();
    ne::PopStyleVar();

    ImGui::Spring(1, 0);
    ImGui::EndVertical();
  }

  ImGui::EndHorizontal();
  ImGui::EndVertical();

  ne::EndNode();

  node_mover.SetNodeSize(node_id, ne::GetNodeSize(node_id));

  if (header_rect.has_value()) {
    auto* drawList = ne::GetNodeBackgroundDrawList(node_id);

    const auto alpha = static_cast<int>(255 * 0.75);
    const auto header_color =
        IM_COL32(0, 0, 0, alpha) |
        (node_data.header->color & IM_COL32(255, 255, 255, 0));
    const auto& style = ne::GetStyle();
    const auto half_border_width = style.NodeBorderWidth * 0.5f;

    header_rect->Min += ImVec2{0.5, 0.5};
    header_rect->Max -= ImVec2{0.5, 0.5};

    drawList->AddRectFilled(
        header_rect->Min - ImVec2{8 - half_border_width, 4 - half_border_width},
        header_rect->Max + ImVec2{8 - half_border_width, 0}, header_color,
        style.NodeRounding, ImDrawFlags_RoundCornersTop);

    drawList->AddLine(
        header_rect->GetBL() + ImVec2{(half_border_width - 8), -0.5},
        header_rect->GetBR() + ImVec2{(8 - half_border_width), -0.5},
        ImColor{255, 255, 255, 96 * alpha / (3 * 255)}, 1.F);
  }

  ImGui::PopID();
  ne::PopStyleVar();
  ImGui::PopStyleVar();
}
}  // namespace vh::ponc::draw