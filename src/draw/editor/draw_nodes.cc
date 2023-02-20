/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#include "draw_nodes.h"

#include "app_state.h"
#include "core_i_node.h"
#include "core_project.h"
#include "coreui_i_node_drawer.h"
#include "coreui_i_pin_drawer.h"
#include "cpp_assert.h"
#include "draw_flow_icon.h"
#include "draw_node_drawer.h"
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
auto IsFlowPin [[nodiscard]] (const coreui::IPinDrawer& pin_drawer) {
  return pin_drawer.GetPinId().has_value();
}

// ---
auto GetPinIconAreaSize [[nodiscard]] (const coreui::IPinDrawer& pin_drawer) {
  if (IsFlowPin(pin_drawer)) {
    return ImVec2{24, 24};
  }

  if (pin_drawer.IsEditable()) {
    return ImVec2{20, 24};
  }

  return ImVec2{24, 24};
}

// ---
void DrawPinIconArea(const coreui::IPinDrawer& pin_drawer) {
  const auto size = GetPinIconAreaSize(pin_drawer);

  if (IsFlowPin(pin_drawer) && ImGui::IsRectVisible(size)) {
    const auto stroke_color = ImColor{255, 255, 255};
    DrawFlowIcon(size, stroke_color, false);
  }

  ImGui::Dummy(size);
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
  for (const auto& node : app_state.project->GetDiagram().GetNodes()) {
    auto& node_flow =
        app_state.flow_calculator->GetCalculatedFlow(node->GetId());
    DrawNode(*node, node_flow);
  }
}

// ---
void Nodes::DrawNode(core::INode& node, flow::NodeFlow& node_flow) {
  const auto node_drawer = node.CreateDrawer();
  auto node_builder = esc::NodeDrawer{node.GetId()};

  if (node_drawer->HasHeader()) {
    const auto header_scope =
        node_builder.AddHeader(node_header_texture_, node_drawer->GetColor());

    DrawHeader(*node_drawer);
  }

  for (const auto& pin_drawer : node_drawer->CreatePinDrawers()) {
    const auto pin_kind = GetPinKind(*pin_drawer, node);

    {
      const auto pin_scope =
          node_builder.AddPin(pin_drawer->GetPinId(), pin_kind);

      if (pin_kind == ne::PinKind::Input) {
        DrawPinIconArea(*pin_drawer);
        DrawPinField(*pin_drawer, node_flow);
      } else {
        DrawPinField(*pin_drawer, node_flow);
        DrawPinIconArea(*pin_drawer);
      }
    }
  }
}
}  // namespace esc::draw