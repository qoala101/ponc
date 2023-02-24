/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#include <memory>
#include <variant>
#include <vector>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <iostream>
#include <optional>
#include <string_view>

#include "app_state.h"
#include "core_diagram.h"
#include "core_i_node.h"
#include "core_project.h"
#include "core_settings.h"
#include "coreui_i_node_drawer.h"
#include "coreui_i_pin_drawer.h"
#include "cpp_assert.h"
#include "cpp_scope.h"
#include "draw_flow_icon.h"
#include "draw_new_link.h"
#include "draw_nodes.h"
#include "draw_widgets.h"
#include "flow_node_flow.h"
#include "flow_tree.h"
#include "frame_node.h"
#include "imgui.h"
#include "imgui_node_editor.h"

namespace esc::draw {
namespace {
// ---
void DrawEmptyPinArea(ImVec2 area_size, bool pin_editable) {
  if (pin_editable) {
    area_size.x -= 4;
  }

  ImGui::Dummy(area_size);
}

// ---
void DrawPinField_v2(const frame::Pin& pin) {
  const auto spring_scope =
      cpp::Scope{[]() { ImGui::Spring(0); }, []() { ImGui::Spring(0); }};

  const auto label = pin.label;

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

void DrawFlowIconPinArea_v2(const frame::Pin& pin, ImVec2 area_size) {
  if (!ImGui::IsRectVisible(area_size)) {
    return;
  }

  DrawFlowIcon(area_size, pin.color, pin.filled);
  ImGui::Dummy(area_size);
}
}  // namespace

// ---
Nodes::Nodes(const Texture& node_header_texture)
    : node_header_texture_{node_header_texture} {}

void Nodes::Draw(const AppState& app_state) {
  drawn_pin_icon_rects_.clear();

  for (const auto& node : app_state.frame.nodes) {
    DrawNode_v2(node);
  }
}

// ---
auto Nodes::GetDrawnPinIconRect(ne::PinId pin_id) const -> const ImRect& {
  return drawn_pin_icon_rects_.at(pin_id.Get());
}

void Nodes::DrawNode_v2(const frame::Node& node) {
  int layout_id_{};

  const auto node_id_ = node.id;

  ne::PushStyleVar(ne::StyleVar_NodePadding, ImVec4{8, 4, 8, 8});
  ne::BeginNode(node_id_);
  ImGui::PushID(node_id_.AsPointer());
  ImGui::BeginVertical("node");

  struct Header {
    draw::Texture texture{};
    ImColor color{};
    ImRect rect{};
  };

  std::optional<Header> header_{};

  if (const auto header = node.header) {
    auto color = header->color;

    header_ = Header{.texture = node_header_texture_, .color = color};

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

      DrawPinIconArea_v2(pin_drawer);
      DrawPinField_v2(pin_drawer);

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

      DrawPinField_v2(pin_drawer);
      DrawPinIconArea_v2(pin_drawer);

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
}

void Nodes::DrawPinIconArea_v2(const frame::Pin& pin) {
  const auto pin_id = pin.id;
  const auto area_size = ImVec2{24, 24};

  if (!pin_id.has_value()) {
    DrawEmptyPinArea(area_size, std::holds_alternative<float*>(pin.value));
    return;
  }

  DrawFlowIconPinArea_v2(pin, area_size);

  drawn_pin_icon_rects_[pin_id->Get()] =
      ImRect{ImGui::GetItemRectMin(), ImGui::GetItemRectMax()};
}
}  // namespace esc::draw