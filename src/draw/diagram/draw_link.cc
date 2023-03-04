#include "imgui_node_editor_internal.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>

#include <variant>

#include "core_link.h"
#include "coreui_link.h"
#include "cpp_assert.h"
#include "draw_link.h"
#include "imgui_node_editor.h"

namespace esc::draw {
namespace {
///
void DrawCoreLink(const coreui::Link& link) {
  Expects(std::holds_alternative<core::Link>(link.type));
  const auto& core_link = std::get<core::Link>(link.type);

  ne::Link(core_link.id, core_link.start_pin_id, core_link.end_pin_id,
           link.color, link.thickness);
}

///
auto CreateFakePin(ne::PinKind pin_kind) {
  auto fake_pin = ax::NodeEditor::Detail::Pin{nullptr, {}, pin_kind};

  const auto& style = ne::GetStyle();

  fake_pin.m_Strength = style.LinkStrength;
  fake_pin.m_Dir = (pin_kind == ne::PinKind::Input) ? style.TargetDirection
                                                    : style.SourceDirection;
  return fake_pin;
}

///
auto GetPos(const std::variant<coreui::MousePos, ImVec2>& pos_variant) {
  if (std::holds_alternative<ImVec2>(pos_variant)) {
    return std::get<ImVec2>(pos_variant);
  }

  return ImGui::GetMousePos();
}

///
void DrawHandmadeLink(const coreui::Link& link) {
  auto fake_link = ax::NodeEditor::Detail::Link{nullptr, {}};
  fake_link.m_Color = link.color;
  fake_link.m_Thickness = link.thickness;

  Expects(std::holds_alternative<coreui::HandmadeLink>(link.type));
  const auto& handmade_link = std::get<coreui::HandmadeLink>(link.type);
  fake_link.m_Start = GetPos(handmade_link.start_pos);
  fake_link.m_End = GetPos(handmade_link.end_pos);

  auto fake_start_pin = CreateFakePin(ne::PinKind::Output);
  fake_link.m_StartPin = &fake_start_pin;

  auto fake_end_pin = CreateFakePin(ne::PinKind::Input);
  fake_link.m_EndPin = &fake_end_pin;

  fake_link.Draw(ImGui::GetWindowDrawList());
}
}  // namespace

///
void DrawLink(const coreui::Link& link) {
  if (std::holds_alternative<core::Link>(link.type)) {
    DrawCoreLink(link);
    return;
  }

  DrawHandmadeLink(link);
}
}  // namespace esc::draw