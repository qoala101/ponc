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

auto GetPos(const coreui::PosVariant &pos_variant,
            const LinkCreation::Callbacks &callbacks) {
  if (std::holds_alternative<ImVec2>(pos_variant)) {
    return std::get<ImVec2>(pos_variant);
  }

  if (std::holds_alternative<ne::PinId>(pos_variant)) {
    return callbacks.get_pin_tip_pos(std::get<ne::PinId>(pos_variant));
  }

  return ImGui::GetMousePos();
}

///
void DrawHandmadeLink(const coreui::HandmadeLink &link,
                      const LinkCreation::Callbacks &callbacks) {
  auto fake_link = ax::NodeEditor::Detail::Link{nullptr, {}};
  fake_link.m_Color = link.color;
  fake_link.m_Thickness = link.thickness;

  auto fake_start_pin = CreateFakePin(ne::PinKind::Output);
  fake_link.m_StartPin = &fake_start_pin;
  fake_link.m_Start = GetPos(link.start_pos, callbacks);

  auto fake_end_pin = CreateFakePin(ne::PinKind::Input);
  fake_link.m_EndPin = &fake_end_pin;
  fake_link.m_End = GetPos(link.end_pos, callbacks);

  fake_link.Draw(ImGui::GetWindowDrawList());
}

// ///
// auto LinkCreation::GetValidNewNodeFamilies(
//     const std::vector<coreui::FamilyGroup>& family_groups) const
//     -> std::vector<coreui::FamilyGroup> {
//   const auto target_pin_kind = core::Pin::GetOppositeKind(source_pin_kind_);

//   auto valid_groups =
//       std::vector<std::pair<std::string, std::vector<const
//       coreui::Family*>>>{};

//   for (const auto& family_group : family_groups) {
//     for (const auto& family : family_group.families) {
//       if (!family.NodesHavePinOfKind(target_pin_kind)) {
//         continue;
//       }

//       const auto valid_group =
//           std::find_if(valid_groups.begin(), valid_groups.end(),
//                        [&family_group](const auto& group) {
//                          return group.first == family_group.label;
//                        });

//       if (valid_group == valid_groups.end()) {
//         valid_groups.emplace_back(family_group.label, std::vector{&family});
//         continue;
//       }

//       valid_group->second.emplace_back(&family);
//     }
//   }

//   return valid_groups;
// }

}  // namespace

void LinkCreation::Draw(coreui::LinkCreation &link_creation,
                        const std::vector<coreui::FamilyGroup> &family_groups,
                        const Callbacks &callbacks) {
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

  if (const auto link = link_creation.GetHandmadeLink()) {
    DrawHandmadeLink(*link, callbacks);
  }

  create_node_popup_.Draw(
      family_groups,
      {.closed = [&link_creation]() { link_creation.DiscardNewNode(); },
       .node_created =
           [&link_creation](auto node) {
             link_creation.AcceptNewNode(std::move(node));
           }});
}

void LinkCreation::DrawNewNodeQuery(coreui::LinkCreation &link_creation,
                                    const ImVec2 &new_node_pos) {
  DrawTooltip("Create Node", {0.F, 1.F / 3, 0.F, 1.F * 3 / 4});

  if (!ne::AcceptNewItem()) {
    return;
  }

  link_creation.SetNodeBeingCreatedAt(new_node_pos);
  create_node_popup_.Open();
}
}  // namespace esc::draw