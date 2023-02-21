/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#include "draw_nodes.h"

#include <iostream>
#include <optional>

#include "app_state.h"
#include "core_i_node.h"
#include "core_project.h"
#include "coreui_i_node_drawer.h"
#include "coreui_i_pin_drawer.h"
#include "cpp_assert.h"
#include "draw_flow_icon.h"
#include "draw_new_link.h"
#include "draw_node_drawer.h"
#include "draw_widgets.h"
#include "flow_calculator.h"
#include "flow_node_flow.h"
#include "imgui.h"
#include "imgui_node_editor.h"

namespace esc::draw {
namespace {
// ---
void DrawHeader(const coreui::INodeDrawer& node_drawer) {
  ImGui::Spring(0);
  ImGui::TextUnformatted(node_drawer.GetLabel().c_str());
  ImGui::Spring(1);
  ImGui::Dummy(ImVec2{0, 28});
  ImGui::Spring(0);
}

// ---
void DrawEmptyPinArea(ImVec2 area_size, bool pin_editable) {
  if (pin_editable) {
    area_size.x -= 4;
  }

  ImGui::Dummy(area_size);
}

// ---
auto GetPinIconAlpha [[nodiscard]] (const std::optional<NewLink>& new_link) {
  auto alpha = 255;

  // if (new_link.has_value()) {
  //   if (!new_link
  //            ->CanConnectToPin(app_state.project.GetDiagram(),
  //                              *pin_drawer.GetPinId())
  //            .valid) {
  //     alpha /= 2;
  //   }
  // }

  return alpha;
}

// ---
auto GetPinIconColor [[nodiscard]] (const std::optional<NewLink>& new_link) {
  const auto color = ImColor{255, 255, 255, GetPinIconAlpha(new_link)};
}

// ---
auto GetFloatValue
    [[nodiscard]] (coreui::IPinDrawer& pin_drawer, flow::NodeFlow& node_flow) {
  const auto float_value = pin_drawer.GetFloat();

  if (float_value.has_value()) {
    return *float_value;
  }

  const auto pin_id = pin_drawer.GetPinId();
  Expects(pin_id.has_value());

  return &flow::GetPinFlow(node_flow, *pin_id);
}

// ---
void DrawPinField(coreui::IPinDrawer& pin_drawer, flow::NodeFlow& node_flow) {
  const auto label = pin_drawer.GetLabel();
  const auto has_label = !label.empty();

  auto* float_value = GetFloatValue(pin_drawer, node_flow);
  const auto has_float = float_value != nullptr;

  if (!has_label && !has_float) {
    return;
  }

  ImGui::Spring(0);

  const auto* label_data = label.c_str();

  if (has_float) {
    if (pin_drawer.IsEditable()) {
      ImGui::SetNextItemWidth(100);
      ImGui::InputFloat(label_data, float_value, 0.F, 0.F, "%.3f");
    } else {
      ImGui::Text("%.3f %s", *float_value, label_data);
    }
  } else {
    ImGui::TextUnformatted(label_data);
  }

  ImGui::Spring(0);
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

  return core::GetPinKind(node, *pin_id);
}
}  // namespace

// ---
Nodes::Nodes(const Texture& node_header_texture)
    : node_header_texture_{node_header_texture} {}

// ---
void Nodes::Draw(const AppState& app_state) {
  for (const auto& node : app_state.project.GetDiagram().GetNodes()) {
    DrawNode(app_state, *node);
  }
}

// ---
auto Nodes::GetPinPosition(ne::PinId pin_id) const -> const ImVec2& {
  std::cout << "pin_positions_.size() " << pin_positions_.size() << "\n";

  for (const auto& a : pin_positions_) {
    std::cout << pin_id.Get() << " " << a.first << "\n";
  }

  return pin_positions_.at(pin_id.Get());
}

// ---
void Nodes::DrawNode(const AppState& app_state, core::INode& node) {
  const auto node_drawer = node.CreateDrawer();
  auto node_builder = esc::NodeDrawer{node.GetId()};

  if (node_drawer->HasHeader()) {
    const auto header_scope =
        node_builder.AddHeader(node_header_texture_, node_drawer->GetColor());

    DrawHeader(*node_drawer);
  }

  auto flow_calculator = flow::FlowCalculator{};
  flow_calculator.Recalculate(app_state.project);
  auto node_flow = flow_calculator.GetCalculatedFlow(node.GetId());

  for (const auto& pin_drawer : node_drawer->CreatePinDrawers()) {
    const auto pin_kind = GetPinKind(*pin_drawer, node);

    {
      const auto pin_scope =
          node_builder.AddPin(pin_drawer->GetPinId(), pin_kind);

      if (pin_kind == ne::PinKind::Input) {
        DrawPinIconArea(app_state, *pin_drawer);
        DrawPinField(*pin_drawer, node_flow);
      } else {
        DrawPinField(*pin_drawer, node_flow);
        DrawPinIconArea(app_state, *pin_drawer);
      }
    }
  }
}

// ---
void Nodes::DrawPinIconArea(const AppState& app_state,
                            const coreui::IPinDrawer& pin_drawer) {
  const auto pin_id = pin_drawer.GetPinId();
  const auto area_size = ImVec2{24, 24};

  if (!pin_id.has_value()) {
    DrawEmptyPinArea(area_size, pin_drawer.IsEditable());
    return;
  }

  pin_positions_[pin_id->Get()] = ImGui::GetItemRectMin();

  if (!ImGui::IsRectVisible(area_size)) {
    return;
  }

  auto alpha = 255;

  if (app_state.widgets.new_link.IsVisible()) {
    if (!app_state.widgets.new_link.CanConnectToPin(app_state,
                                                    *pin_drawer.GetPinId())) {
      alpha /= 2;
    }
  }

  const auto color = ImColor{255, 255, 255, alpha};
  const auto filled =
      FindPinLink(app_state.project.GetDiagram(), *pin_drawer.GetPinId())
          .has_value();
  DrawFlowIcon(area_size, color, filled);
  ImGui::Dummy(area_size);

  // pin_positions_[pin_id->Get()] = ImGui::GetItemRectMin();

  // if (new_link_->has_value()) {
  //   if ((*new_link_)->rebind.has_value()) {
  //     if ((*new_link_)->rebind->fixed_pin == pin_id) {
  //       const auto rect =
  //           ImRect{ImGui::GetItemRectMin(), ImGui::GetItemRectMax()};

  //       const auto end_pos =
  //           (pin_kind == ax::NodeEditor::PinKind::Input)
  //               ? ImVec2{rect.Min.x, (rect.Min.y + rect.Max.y) * 0.5f}
  //               : ImVec2{rect.Max.x, (rect.Min.y + rect.Max.y) * 0.5f};

  //       (*new_link_)->rebind->fixed_pin_pos = end_pos;
  //     }
  //   }
  // }
}
}  // namespace esc::draw