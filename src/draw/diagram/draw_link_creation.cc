#include <variant>

#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "coreui_link_creation.h"
#include "draw_link_creation.h"
#include "draw_tooltip.h"
#include "imgui_node_editor.h"
#include "imgui_node_editor_internal.h"

namespace esc::draw {
namespace {
///
auto GetNewLinkAlpha(const coreui::LinkCreation &link_creation) {
  return link_creation.GetManualLink().has_value() ? 0.F : 1.F;
}

///
void DrawNewLinkQuery(coreui::LinkCreation &link_creation) {
  const auto [can_create_link, reason] = link_creation.GetCanCreateLinkReason();
  const auto new_link_alpha = GetNewLinkAlpha(link_creation);

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

  link_creation.AcceptNewLink();
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
void LinkCreation::Draw(coreui::LinkCreation &link_creation,
                        const Callbacks &callbacks) {
  const auto mouse_pos = ImGui::GetMousePos();

  if (ne::BeginCreate({1.F, 1.F, 1.F, GetNewLinkAlpha(link_creation)}, 3)) {
    auto dragged_from_pin = ne::PinId{};
    auto hovering_over_pin = ne::PinId{};

    if (ne::QueryNewLink(&dragged_from_pin, &hovering_over_pin)) {
      link_creation.SetPins(dragged_from_pin, hovering_over_pin);
      DrawNewLinkQuery(link_creation);
    } else if (ne::QueryNewNode(&dragged_from_pin)) {
      new_node_pos_ = mouse_pos;
      link_creation.SetPins(dragged_from_pin);
      DrawNewNodeQuery(link_creation, callbacks);
    }
  } else if (!link_creation.IsCreatingNode()) {
    link_creation.SetPins({});
  }

  ne::EndCreate();

  if (const auto link = link_creation.GetManualLink()) {
    DrawManualLink(**link, callbacks);
  }
}

///
void LinkCreation::DrawNewNodeQuery(coreui::LinkCreation &link_creation,
                                    const Callbacks &callbacks) const {
  DrawTooltip("Create Node", {0.F, 1.F / 3, 0.F, 1.F * 3 / 4});

  if (!ne::AcceptNewItem()) {
    return;
  }

  link_creation.StartCreatingNode();
  callbacks.new_node_requested_at(new_node_pos_);
}

///
auto LinkCreation::GetPosValue(const coreui::PosVariant &pos_variant,
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
void LinkCreation::DrawManualLink(const coreui::ManualLink &link,
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