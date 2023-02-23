/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#include <memory>
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
void DrawPinField(coreui::IPinDrawer& pin_drawer,
                  const flow::NodeFlow& node_flow) {
  const auto spring_scope =
      cpp::Scope{[]() { ImGui::Spring(0); }, []() { ImGui::Spring(0); }};

  const auto label = pin_drawer.GetLabel();

  if (const auto float_value = pin_drawer.GetFloat()) {
    if (pin_drawer.IsEditable()) {
      ImGui::SetNextItemWidth(100);
      ImGui::InputFloat(label.c_str(), *float_value, 0.F, 0.F, "%.3f");
      return;
    }

    ImGui::Text("%.3f %s", **float_value, label.c_str());
    return;
  }

  if (const auto pin_id = pin_drawer.GetPinId()) {
    const auto pin_flow = GetPinFlow(node_flow, *pin_id);
    ImGui::Text("%.3f %s", pin_flow, label.c_str());
    return;
  }

  ImGui::TextUnformatted(label.c_str());
}

// ---
auto GetPinKind [[nodiscard]] (const coreui::IPinDrawer& pin_drawer,
                               const core::INode& node) {
  const auto pin_kind = pin_drawer.GetKind();

  if (pin_kind.has_value()) {
    return *pin_kind;
  }

  const auto pin_id = pin_drawer.GetPinId();
  Expects(pin_id.has_value());

  return GetPinKind(node, *pin_id);
}

// ---
auto GetPinIconAlpha
    [[nodiscard]] (ne::PinId pin_id, const core::Diagram& diagram,
                   const NewLink& new_link) {
  auto alpha = 1.F;

  if (const auto creating_new_link = new_link.GetDraggedFromPin().has_value()) {
    if (!new_link.CanConnectToPin(pin_id, diagram)) {
      alpha /= 4;
    }
  }

  return alpha;
}

void DrawFlowIconPinArea(ne::PinId pin_id, ImVec2 area_size,
                         const flow::NodeFlow& node_flow,
                         const core::Diagram& diagram, const NewLink& new_link,
                         const SettingsView& settings_view,
                         const core::Settings& settings) {
  if (!ImGui::IsRectVisible(area_size)) {
    return;
  }

  const auto alpha = GetPinIconAlpha(pin_id, diagram, new_link);

  const auto start_pin_flow = flow::GetPinFlow(node_flow, pin_id);
  auto color = settings_view.GetColorForFlowValue(start_pin_flow, settings);
  color.Value.w = alpha;

  const auto filled = FindPinLink(diagram, pin_id).has_value();
  DrawFlowIcon(area_size, color, filled);
  ImGui::Dummy(area_size);
}
}  // namespace

// ---
Nodes::Nodes(const Texture& node_header_texture)
    : node_header_texture_{node_header_texture} {}

void Nodes::Draw(const AppState& app_state) {
  drawn_pin_icon_rects_.clear();

  const auto& diagram = app_state.project.GetDiagram();
  const auto& nodes = diagram.GetNodes();
  const auto flow_tree = flow::BuildFlowTree(diagram);
  const auto node_flows = flow::CalculateNodeFlows(flow_tree);
  const auto& new_link = app_state.widgets.new_link;

  for (const auto& node : nodes) {
    const auto& node_flow = node_flows.at(node->GetId().Get());
    DrawNode(*node, node_flow, diagram, new_link,
             app_state.widgets.settings_view, app_state.project.GetSettings());
  }
}

// ---
auto Nodes::GetDrawnPinIconRect(ne::PinId pin_id) const -> const ImRect& {
  return drawn_pin_icon_rects_.at(pin_id.Get());
}

// ---
void Nodes::DrawNode(core::INode& node, const flow::NodeFlow& node_flow,
                     const core::Diagram& diagram, const NewLink& new_link,
                     const SettingsView& settings_view,
                     const core::Settings& settings) {
  const auto node_drawer = node.CreateDrawer();
  int layout_id_{};

  const auto node_id_ = node.GetId();

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

  if (node_drawer->HasHeader()) {
    auto color = node_drawer->GetColor();

    if (settings_view.IsColorFlow()) {
      if (const auto input_flow = node_flow.input_pin_flow) {
        color =
            settings_view.GetColorForFlowValue(input_flow->second, settings);
      } else {
        color = ImColor{1.F, 1.F, 1.F};
      }
    }

    header_ = Header{.texture = node_header_texture_, .color = color};

    ImGui::BeginHorizontal("header");

    ImGui::Spring(0);
    ImGui::TextUnformatted(node_drawer->GetLabel().c_str());
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

  auto pin_drawers = node_drawer->CreatePinDrawers();
  auto input_pin_drawers = std::vector<std::unique_ptr<coreui::IPinDrawer>>{};
  auto output_pin_drawers = std::vector<std::unique_ptr<coreui::IPinDrawer>>{};

  for (auto& pin_drawer : pin_drawers) {
    const auto pin_kind = GetPinKind(*pin_drawer, node);

    if (pin_kind == ne::PinKind::Input) {
      input_pin_drawers.emplace_back(std::move(pin_drawer));
      continue;
    }

    output_pin_drawers.emplace_back(std::move(pin_drawer));
  }

  if (!input_pin_drawers.empty()) {
    ImGui::BeginVertical("inputs", {0, 0}, 0.0F);
    ne::PushStyleVar(ne::StyleVar_PivotAlignment, {0, 0.5F});
    ne::PushStyleVar(ne::StyleVar_PivotSize, {0, 0});

    // if (!header_.has_value()) {
    //   ImGui::Spring(1, 0);
    // }

    for (const auto& pin_drawer : input_pin_drawers) {
      const auto pin_kind = GetPinKind(*pin_drawer, node);
      const auto id = pin_drawer->GetPinId();
      const auto kind = pin_kind;

      if (id.has_value()) {
        ne::BeginPin(*id, kind);
        ImGui::BeginHorizontal(id->AsPointer());
      } else {
        ImGui::BeginHorizontal(layout_id_++);
      }

      DrawPinIconArea(*pin_drawer, node_flow, diagram, new_link, settings_view,
                      settings);
      DrawPinField(*pin_drawer, node_flow);

      ImGui::EndHorizontal();

      if (id.has_value()) {
        ne::EndPin();
      }
    }

    ne::PopStyleVar(2);
    ImGui::Spring(1, 0);
    ImGui::EndVertical();
  }

  if (!output_pin_drawers.empty()) {
    if (!input_pin_drawers.empty()) {
      ImGui::Spring(1);
    }

    ImGui::BeginVertical("outputs", {0, 0}, 1.0F);
    ne::PushStyleVar(ne::StyleVar_PivotAlignment, {1.0f, 0.5f});
    ne::PushStyleVar(ne::StyleVar_PivotSize, {0, 0});

    // if (!header_.has_value()) {
    //   ImGui::Spring(1, 0);
    // }

    for (const auto& pin_drawer : output_pin_drawers) {
      const auto pin_kind = GetPinKind(*pin_drawer, node);
      const auto id = pin_drawer->GetPinId();
      const auto kind = pin_kind;

      if (id.has_value()) {
        ne::BeginPin(*id, kind);
        ImGui::BeginHorizontal(id->AsPointer());
      } else {
        ImGui::BeginHorizontal(layout_id_++);
      }

      DrawPinField(*pin_drawer, node_flow);
      DrawPinIconArea(*pin_drawer, node_flow, diagram, new_link, settings_view,
                      settings);

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

// ---
void Nodes::DrawPinIconArea(const coreui::IPinDrawer& pin_drawer,
                            const flow::NodeFlow& node_flow,
                            const core::Diagram& diagram,
                            const NewLink& new_link,
                            const SettingsView& settings_view,
                            const core::Settings& settings) {
  const auto pin_id = pin_drawer.GetPinId();
  const auto area_size = ImVec2{24, 24};

  if (!pin_id.has_value()) {
    DrawEmptyPinArea(area_size, pin_drawer.IsEditable());
    return;
  }

  DrawFlowIconPinArea(*pin_id, area_size, node_flow, diagram, new_link,
                      settings_view, settings);

  drawn_pin_icon_rects_[pin_id->Get()] =
      ImRect{ImGui::GetItemRectMin(), ImGui::GetItemRectMax()};
}
}  // namespace esc::draw