#define IMGUI_DEFINE_MATH_OPERATORS
#include "draw_linker.h"

#include <variant>

#include "coreui_linker.h"
#include "draw_connect_node_popup.h"
#include "draw_tooltip.h"
#include "imgui.h"
#include "imgui_node_editor.h"
#include "imgui_node_editor_internal.h"

namespace esc::draw {
namespace {
///
void DrawNewLinkQuery(coreui::Linker &linker, float new_link_alpha) {
  const auto [can_create_link, reason] = linker.GetCanCreateLinkReason();

  if (!can_create_link) {
    DrawTooltip(reason, {1.F / 3, 0.F, 0.F, 1.F * 3 / 4});
    ne::RejectNewItem({1.F, 1.F / 2, 1.F / 2, new_link_alpha}, 4);
    return;
  }

  if (reason.empty()) {
    return;
  }

  DrawTooltip(reason.c_str(), {0.F, 1.F / 3, 0.F, 1.F * 3 / 4});

  if (!ne::AcceptNewItem({1.F / 2, 1.F, 1.F / 2, new_link_alpha}, 4)) {
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

  if (ne::BeginCreate({1.F, 1.F, 1.F, new_link_alpha}, 3)) {
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
  DrawTooltip(ConnectNodePopup::GetLabel(), {0.F, 1.F / 3, 0.F, 1.F * 3 / 4});

  if (!ne::AcceptNewItem()) {
    return;
  }

  linker.StartCreatingNodeAt(new_node_pos);
  connect_node_popup_.Open();
}
}  // namespace esc::draw