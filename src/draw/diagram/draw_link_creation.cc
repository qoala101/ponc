#include "draw_link_creation.h"

#include "draw_tooltip.h"
#include "imgui_node_editor.h"

namespace esc::draw {
namespace {
///
auto GetNewLinkAlpha(const coreui::LinkCreation &link_creation) {
  return (link_creation.IsCreatingLink() && link_creation.IsRepinningLink())
             ? 0.F
             : 1.F;
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

  link_creation.AcceptCurrentLink();
}
}  // namespace

///
void LinkCreation::Draw(coreui::LinkCreation &link_creation,
                        const std::vector<coreui::FamilyGroup> &family_groups) {
  const auto mouse_pos = ImGui::GetMousePos();

  if (ne::BeginCreate({1.F, 1.F, 1.F, GetNewLinkAlpha(link_creation)}, 3)) {
    auto dragged_from_pin = ne::PinId{};
    auto hovering_over_pin = ne::PinId{};

    if (ne::QueryNewLink(&dragged_from_pin, &hovering_over_pin)) {
      link_creation.SetPins(dragged_from_pin, hovering_over_pin);
      DrawNewLinkQuery(link_creation);
    } else if (ne::QueryNewNode(&dragged_from_pin)) {
      link_creation.SetPins(dragged_from_pin);
      DrawNewNodeQuery(link_creation, mouse_pos);
    }
  } else {
    link_creation.SetPins({});
  }

  ne::EndCreate();

  connection_popup_.Draw(family_groups);
}

///
void LinkCreation::DrawNewNodeQuery(coreui::LinkCreation &link_creation,
                                    const ImVec2 &new_node_pos) {
  DrawTooltip("Create Node", {0.F, 1.F / 3, 0.F, 1.F * 3 / 4});

  if (!ne::AcceptNewItem()) {
    return;
  }

  const auto [source_pin_id, source_pin_kind] =
      link_creation.GetCurrentLinkSourcePin();

  connection_popup_.SetSourcePin(source_pin_id, source_pin_kind);
  connection_popup_.SetPos(new_node_pos);
  connection_popup_.Show();
}
}  // namespace esc::draw