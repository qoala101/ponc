#include <cstdint>
#include <memory>
#include <unordered_map>
#include <variant>
#include <vector>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <iostream>
#include <optional>
#include <string_view>

#include "coreui_project.h"
#include "cpp_assert.h"
#include "cpp_scope.h"
#include "draw_flow_icon.h"
#include "draw_node.h"
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
  ImGui::InputFloat(label.c_str(), float_value, 0, 0, "%.3f");
}

auto DrawPinIconArea(const coreui::Pin& pin) -> std::optional<ImRect> {
  const auto pin_id = pin.icon;
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

  DrawFlowIcon(area_size, pin.icon->GetData().color,
               pin.icon->GetData().filled);
  ImGui::Dummy(area_size);

  return ImRect{ImGui::GetItemRectMin(), ImGui::GetItemRectMax()};
}
}  // namespace

void DrawNode(coreui::Node& node) {
  ne::SetNodePosition(node.id, node.pos);

  ne::PushStyleVar(ne::StyleVar_NodePadding, ImVec4{8, 4, 8, 8});
  ne::BeginNode(node.id);

  ImGui::PushID(node.id.AsPointer());
  ImGui::BeginVertical("Node");

  auto header_rect = std::optional<ImRect>{};

  if (const auto header = node.header) {
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

  if (!node.input_pins.empty()) {
    ImGui::BeginVertical("InputPins", {0, 0}, 0);

    // if (!header_.has_value()) {
    //   ImGui::Spring(1, 0);
    // }

    ne::PushStyleVar(ne::StyleVar_PivotAlignment, {0, 0.5F});
    ne::PushStyleVar(ne::StyleVar_PivotSize, {0, 0});

    for (auto& pin : node.input_pins) {
      if (pin.icon.has_value()) {
        ne::BeginPin(pin.icon->GetData().id, ne::PinKind::Input);
        ImGui::BeginHorizontal(pin.icon->GetData().id.AsPointer());
      } else {
        ImGui::BeginHorizontal(layout_id++);
      }

      pin.icon->SetRect(*DrawPinIconArea(pin));
      DrawPinField(pin);

      ImGui::EndHorizontal();

      if (pin.icon.has_value()) {
        ne::EndPin();
      }
    }

    ne::PopStyleVar();
    ne::PopStyleVar();

    ImGui::Spring(1, 0);
    ImGui::EndVertical();
  }

  if (!node.output_pins.empty()) {
    if (!node.input_pins.empty()) {
      ImGui::Spring(1);
    }

    ImGui::BeginVertical("OutputPins", {0, 0}, 1);

    // if (!header_.has_value()) {
    //   ImGui::Spring(1, 0);
    // }

    ne::PushStyleVar(ne::StyleVar_PivotAlignment, {1, 0.5F});
    ne::PushStyleVar(ne::StyleVar_PivotSize, {0, 0});

    for (auto& pin : node.output_pins) {
      if (pin.icon.has_value()) {
        ne::BeginPin(pin.icon->GetData().id, ne::PinKind::Output);
        ImGui::BeginHorizontal(pin.icon->GetData().id.AsPointer());
      } else {
        ImGui::BeginHorizontal(layout_id++);
      }

      DrawPinField(pin);
      pin.icon->SetRect(*DrawPinIconArea(pin));

      ImGui::EndHorizontal();

      if (pin.icon.has_value()) {
        ne::EndPin();
      }
    }

    ne::PopStyleVar();
    ne::PopStyleVar();

    ImGui::Spring(1, 0);
    ImGui::EndVertical();
  }

  ImGui::EndHorizontal();
  ImGui::EndVertical();

  ne::EndNode();

  if (header_rect.has_value() && ImGui::IsItemVisible()) {
    auto* drawList = ne::GetNodeBackgroundDrawList(node.id);

    const auto texture_uv =
        ImVec2{header_rect->GetWidth() /
                   (4.0F * static_cast<float>(node.header->texture.width)),
               header_rect->GetHeight() /
                   (4.0F * static_cast<float>(node.header->texture.height))};
    const auto alpha = static_cast<int>(255 * ImGui::GetStyle().Alpha);
    const auto header_color = IM_COL32(0, 0, 0, alpha) |
                              (node.header->color & IM_COL32(255, 255, 255, 0));
    const auto& style = ne::GetStyle();
    const auto half_border_width = style.NodeBorderWidth * 0.5f;

    drawList->AddImageRounded(
        node.header->texture.id,
        header_rect->Min - ImVec2{8 - half_border_width, 4 - half_border_width},
        header_rect->Max + ImVec2{8 - half_border_width, 0}, ImVec2{0.0F, 0.0F},
        texture_uv, header_color, style.NodeRounding,
        ImDrawFlags_RoundCornersTop);

    drawList->AddLine(
        header_rect->GetBL() + ImVec2{(half_border_width - 8), -0.5F},
        header_rect->GetBR() + ImVec2{(8 - half_border_width), -0.5F},
        ImColor{255, 255, 255, 96 * alpha / (3 * 255)}, 1.0F);
  }

  ImGui::PopID();
  ne::PopStyleVar();
}
}  // namespace esc::draw