#include <variant>

#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "coreui_linking.h"
#include "draw_linking.h"
#include "draw_tooltip.h"
#include "imgui_node_editor.h"
#include "imgui_node_editor_internal.h"

namespace esc::draw {
namespace {
///
auto GetNewLinkAlpha(const coreui::Linking &linking) {
  return linking.GetManualLink().has_value() ? 0.F : 1.F;
}

///
void DrawNewLinkQuery(coreui::Linking &linking) {
  const auto [can_create_link, reason] = linking.GetCanCreateLinkReason();
  const auto new_link_alpha = GetNewLinkAlpha(linking);

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

  linking.AcceptNewLink();
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
}  // namespace

///
void Linking::Draw(coreui::Linking &linking, const Callbacks &callbacks) {
  const auto mouse_pos = ImGui::GetMousePos();

  if (ne::BeginCreate({1.F, 1.F, 1.F, GetNewLinkAlpha(linking)}, 3)) {
    auto dragged_from_pin = ne::PinId{};
    auto hovering_over_pin = ne::PinId{};

    if (ne::QueryNewLink(&dragged_from_pin, &hovering_over_pin)) {
      linking.SetPins(dragged_from_pin, hovering_over_pin);
      DrawNewLinkQuery(linking);
    } else if (ne::QueryNewNode(&dragged_from_pin)) {
      new_node_pos_ = mouse_pos;
      linking.SetPins(dragged_from_pin);
      DrawNewNodeQuery(linking, callbacks);
    }
  } else if (!linking.IsCreatingNode()) {
    linking.SetPins({});
  }

  ne::EndCreate();

  if (const auto link = linking.GetManualLink()) {
    DrawManualLink(**link, callbacks);
  }
}

///
void Linking::DrawNewNodeQuery(coreui::Linking &linking,
                               const Callbacks &callbacks) const {
  DrawTooltip("Create Node", {0.F, 1.F / 3, 0.F, 1.F * 3 / 4});

  if (!ne::AcceptNewItem()) {
    return;
  }

  linking.StartCreatingNode();
  callbacks.new_node_requested_at(new_node_pos_);
}

///
auto Linking::GetPosValue(const coreui::PosVariant &pos_variant,
                          const Callbacks &callbacks) const {
  if (std::holds_alternative<coreui::NewNodePos>(pos_variant)) {
    return new_node_pos_;
  }

  if (std::holds_alternative<ne::PinId>(pos_variant)) {
    return callbacks.get_pin_tip_pos(std::get<ne::PinId>(pos_variant));
  }

  return ImGui::GetMousePos();
}

///
void Linking::DrawManualLink(const coreui::ManualLink &link,
                             const Callbacks &callbacks) const {
  auto fake_link = ax::NodeEditor::Detail::Link{nullptr, {}};
  fake_link.m_Color = link.color;
  fake_link.m_Thickness = link.thickness;

  auto fake_start_pin = CreateFakePin(ne::PinKind::Output);
  fake_link.m_StartPin = &fake_start_pin;
  fake_link.m_Start = GetPosValue(link.start_pos, callbacks);

  auto fake_end_pin = CreateFakePin(ne::PinKind::Input);
  fake_link.m_EndPin = &fake_end_pin;
  fake_link.m_End = GetPosValue(link.end_pos, callbacks);

  fake_link.Draw(ImGui::GetWindowDrawList());
}
}  // namespace esc::draw