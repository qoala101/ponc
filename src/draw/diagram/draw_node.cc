/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_node.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_node_editor.h>

#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "core_i_node.h"
#include "core_id_value.h"
#include "coreui_i_pin_traits.h"
#include "coreui_node.h"
#include "coreui_pin.h"
#include "cpp_assert.h"
#include "cpp_scope.h"
#include "draw_colored_text.h"
#include "draw_flow_icon.h"
#include "style_utils.h"

namespace vh::ponc::draw {
namespace {
///
void DrawSmallSpace() {
  // ImGui::Spring(0);
}

///
void DrawGrowingSpace() {
  // ImGui::Spring(1, 0);
}

///
void DrawPinField(const coreui::Pin& pin, ne::PinKind pin_kind) {
  const auto has_label = pin.label.has_value() && !pin.label->text.empty();
  const auto has_value = !std::holds_alternative<std::monostate>(pin.value);

  if (!has_label && !has_value) {
    return;
  }

  DrawSmallSpace();

  const auto add_space_scope = cpp::Scope{[pin_kind]() {
    if (pin_kind == ne::PinKind::Output) {
      DrawGrowingSpace();
    }
  }};

  if (!has_value) {
    return;
  }

  ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4);
  const auto frame_rounding_scope = cpp::Scope{[]() { ImGui::PopStyleVar(); }};
  const auto spring_scope = cpp::Scope{[]() { DrawSmallSpace(); }};

  if (std::holds_alternative<float>(pin.value)) {
    const auto float_value = std::get<float>(pin.value);
    ImGui::Text("%.2f", float_value);
    return;
  }

  Expects(std::holds_alternative<float*>(pin.value));
  auto* editable_value = std::get<float*>(pin.value);

  const auto input_width = 75;
  ImGui::SetNextItemWidth(input_width);
  ImGui::InputFloat("", editable_value, 0, 0, "%.2f");
}

///
void DrawPinIconArea(const coreui::Pin& pin) {
  const auto pin_id = pin.flow_data;
  const auto frame_height = ImGui::GetFrameHeight();
  auto area_size = ImVec2{frame_height, frame_height};

  if (!pin_id.has_value()) {
    if (std::holds_alternative<float*>(pin.value)) {
      area_size.x -= ImGui::GetStyle().FramePadding.x;
    }

    ImGui::Dummy(area_size);
    return;
  }

  DrawFlowIcon(area_size, pin.flow_data->color, pin.flow_data->filled);
  ImGui::Dummy(area_size);
}

///
auto GetNodeAlpha() { return ne::GetStyle().Colors[ne::StyleColor_NodeBg].w; }

///
void DrawPinLabel(const std::optional<coreui::PinLabel>& pin_label,
                  ne::PinKind pin_kind) {
  if (!pin_label.has_value()) {
    return;
  }

  if (pin_kind == ne::PinKind::Output) {
    DrawSmallSpace();
  }

  const auto color = style::WithAlpha(pin_label->color, GetNodeAlpha());
  const auto frame_padding = ImGui::GetStyle().FramePadding;

  auto pos = ImGui::GetCursorPos();
  pos.y += frame_padding.y;

  DrawColoredText(pin_label->text, color, pos);

  if (pin_kind == ne::PinKind::Input) {
    DrawSmallSpace();
    return;
  }

  pos = ImGui::GetCursorPos();
  pos.y -= frame_padding.y;
  ImGui::SetCursorPos(pos);
}

///
auto DrawHeader(const std::optional<coreui::Header>& header)
    -> std::optional<ImRect> {
  if (!header.has_value()) {
    return std::nullopt;
  }

  // ImGui::BeginHorizontal("Header");
  DrawSmallSpace();

  // ImGui::BeginVertical("Label");
  ImGui::TextUnformatted(header->label.c_str());
  DrawSmallSpace();

  // ImGui::EndVertical();
  // ImGui::EndHorizontal();

  return ImRect{ImGui::GetItemRectMin(), ImGui::GetItemRectMax()};
}

///
auto DrawHeaderBackground(std::optional<ImRect> header_rect,
                          const std::optional<coreui::Header>& header,
                          ne::NodeId node_id) {
  if (!header_rect.has_value()) {
    return;
  }

  const auto& style = ne::GetStyle();

  const auto padding = style.NodePadding;
  header_rect->Min -= ImVec2{padding.x, padding.y};
  header_rect->Max += ImVec2{padding.z, 0};

  const auto border = style.NodeBorderWidth;
  header_rect->Min += ImVec2{border, border};
  header_rect->Max -= ImVec2{border, 0};

  Expects(header.has_value());
  const auto color = style::WithAlpha(header->color, GetNodeAlpha());

  auto* draw_list = ne::GetNodeBackgroundDrawList(node_id);
  Expects(draw_list != nullptr);

  draw_list->AddRectFilled(header_rect->Min, header_rect->Max, color,
                           style.NodeRounding, ImDrawFlags_RoundCornersTop);
}

///
void DrawInputPins(const coreui::NodeData& node_data) {
  if (node_data.input_pins.empty()) {
    return;
  }

  // ImGui::BeginVertical("InputPins");
  ne::PushStyleVar(ne::StyleVar_PivotAlignment, {0, 0.5});

  auto pin_layout_id = 0;

  for (const auto& pin : node_data.input_pins) {
    // ImGui::BeginHorizontal(pin_layout_id++);

    const auto pin_is_flow = pin.flow_data.has_value();

    if (pin_is_flow) {
      ne::BeginPin(pin.flow_data->id, ne::PinKind::Input);
    }

    DrawPinIconArea(pin);
    DrawPinField(pin, ne::PinKind::Input);

    if (pin_is_flow) {
      ne::EndPin();
    }

    DrawPinLabel(pin.label, ne::PinKind::Input);
    // ImGui::EndHorizontal();
  }

  DrawGrowingSpace();

  ne::PopStyleVar();
  // ImGui::EndVertical();
}

///
void DrawOutputPins(const coreui::NodeData& node_data) {
  if (node_data.output_pins.empty()) {
    return;
  }

  // ImGui::BeginVertical("OutputPins");
  ne::PushStyleVar(ne::StyleVar_PivotAlignment, {1, 0.5});

  auto pin_poses = std::unordered_map<core::IdValue<ne::PinId>, ImVec2>{};
  auto pin_layout_id = 0;

  for (const auto& pin : node_data.output_pins) {
    // ImGui::BeginHorizontal(pin_layout_id++);

    DrawPinLabel(pin.label, ne::PinKind::Output);

    const auto pin_is_flow = pin.flow_data.has_value();

    if (pin_is_flow) {
      ne::BeginPin(pin.flow_data->id, ne::PinKind::Output);
    }

    DrawPinField(pin, ne::PinKind::Output);
    DrawPinIconArea(pin);

    if (pin_is_flow) {
      ne::EndPin();
    }

    // ImGui::EndHorizontal();
  }

  DrawGrowingSpace();

  ne::PopStyleVar();
  // ImGui::EndVertical();
}

///
void DrawBody(const coreui::NodeData& node_data) {
  // ImGui::BeginVertical("Body");

  if (node_data.header.has_value()) {
    DrawSmallSpace();
  }

  // ImGui::BeginHorizontal("Pins");

  DrawInputPins(node_data);
  DrawOutputPins(node_data);

  // ImGui::EndHorizontal();
  // ImGui::EndVertical();
}
}  // namespace

///
void DrawNode(coreui::Node& node, coreui::NodeMover& node_mover) {
  auto& core_node = node.GetNode();
  const auto node_id = core_node.GetId();

  ImGui::PushID(node_id.AsPointer());

  ne::BeginNode(node_id);
  // ImGui::BeginVertical("Node");

  const auto& node_data = node.GetData();
  const auto header_rect = DrawHeader(node_data.header);
  DrawBody(node_data);

  // ImGui::EndVertical();
  ne::EndNode();

  DrawHeaderBackground(header_rect, node_data.header, node_id);

  ImGui::PopID();

  core_node.SetPos(ne::GetNodePosition(node_id));
  node_mover.SetItemSize(node_id, ne::GetNodeSize(node_id));
}
}  // namespace vh::ponc::draw