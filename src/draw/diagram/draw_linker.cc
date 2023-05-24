/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "style_utils.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_node_editor.h>
#include <imgui_node_editor_internal.h>

#include <string>
#include <variant>

#include "coreui_linker.h"
#include "draw_connect_node_popup.h"
#include "draw_linker.h"
#include "draw_tooltip.h"
#include "style_default_colors.h"

namespace vh::ponc::draw {
namespace {
///
void DrawNewLinkQuery(coreui::Linker &linker, float new_link_alpha) {
  const auto [can_create_link, reason] = linker.GetCanCreateLinkReason();

  if (!can_create_link) {
    DrawTooltip(reason, style::WithAlpha(style::DefaultColors::kError, 0.75));
    ne::RejectNewItem(
        style::WithAlpha(style::DefaultColors::kError, new_link_alpha), 4);
    return;
  }

  if (reason.empty()) {
    return;
  }

  DrawTooltip(reason, style::WithAlpha(style::DefaultColors::kSuccess, 0.75));

  if (!ne::AcceptNewItem(
          style::WithAlpha(style::DefaultColors::kSuccess, new_link_alpha),
          4)) {
    return;
  }

  linker.AcceptCreateLink();
}

///
auto CreateFakePin(ne::PinKind pin_kind) {
  auto fake_pin = ax::NodeEditor::Detail::Pin{nullptr, {}, pin_kind};

  const auto &style = ne::GetStyle();

  fake_pin.m_Strength = style.LinkStrength;
  fake_pin.m_Dir = (pin_kind == ne::PinKind::Input) ? style.TargetDirection
                                                    : style.SourceDirection;
  return fake_pin;
}

///
auto GetPosValue(const coreui::PosVariant &pos_variant) {
  if (std::holds_alternative<ImVec2>(pos_variant)) {
    return std::get<ImVec2>(pos_variant);
  }

  return ImGui::GetMousePos();
}

///
void DrawManualLink(const coreui::ManualLink &link) {
  auto fake_link = ax::NodeEditor::Detail::Link{nullptr, {}};
  fake_link.m_Color = link.color;
  fake_link.m_Thickness = link.thickness;

  auto fake_start_pin = CreateFakePin(ne::PinKind::Output);
  fake_link.m_StartPin = &fake_start_pin;
  fake_link.m_Start = GetPosValue(link.start_pos);

  auto fake_end_pin = CreateFakePin(ne::PinKind::Input);
  fake_link.m_EndPin = &fake_end_pin;
  fake_link.m_End = GetPosValue(link.end_pos);

  fake_link.Draw(ImGui::GetWindowDrawList());
}
}  // namespace

///
void Linker::Draw(coreui::Linker &linker,
                  const std::vector<coreui::FamilyGroup> &family_groups) {
  const auto mouse_pos = ImGui::GetMousePos();
  const auto &manual_links = linker.GetManualLinks();
  const auto new_link_alpha = manual_links.empty() ? 1.F : 0.F;

  if (ne::BeginCreate(
          style::WithAlpha(style::DefaultColors::kWhite, new_link_alpha), 3)) {
    auto dragged_from_pin = ne::PinId{};
    auto hovering_over_pin = ne::PinId{};

    if (ne::QueryNewLink(&dragged_from_pin, &hovering_over_pin)) {
      linker.SetPins(dragged_from_pin, hovering_over_pin);
      DrawNewLinkQuery(linker, new_link_alpha);
    } else if (ne::QueryNewNode(&dragged_from_pin)) {
      linker.SetPins(dragged_from_pin);
      DrawNewNodeQuery(linker, mouse_pos);
    }
  } else {
    linker.SetPins({});
  }

  ne::EndCreate();

  for (const auto &link : linker.GetManualLinks()) {
    DrawManualLink(link);
  }

  connect_node_popup_.Draw(linker, family_groups);
}

///
void Linker::DrawNewNodeQuery(coreui::Linker &linker,
                              const ImVec2 &new_node_pos) {
  DrawTooltip(ConnectNodePopup::GetLabel(),
              style::WithAlpha(style::DefaultColors::kSuccess, 0.75));

  if (!ne::AcceptNewItem()) {
    return;
  }

  linker.StartCreatingNodeAt(new_node_pos);
  connect_node_popup_.Open();
}
}  // namespace vh::ponc::draw