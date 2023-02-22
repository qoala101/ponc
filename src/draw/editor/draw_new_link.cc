#include "draw_new_link.h"

#include <iostream>

#include "app_event_queue.h"
#include "app_events.h"
#include "core_diagram.h"
#include "core_project.h"
#include "cpp_assert.h"
#include "cpp_scope.h"
#include "draw_tooltip.h"
#include "draw_widgets.h"
#include "imgui_node_editor.h"

namespace esc::draw {
namespace {
struct RebindingLink {
  ne::PinId fixed_pin{};
  ne::PinKind fixed_pin_kind{};
  ne::LinkId link_id{};
  std::optional<ImVec2> fixed_pin_pos{};
};

struct HoveredOverLink {
  ne::PinId fixed_pin{};
  ne::PinKind fixed_pin_kind{};
  ne::LinkId link_id{};
};

// ---
void RejectNewLink(std::string_view tooltip, float alpha) {
  DrawTooltip(tooltip, ImColor{1.F / 3, 0.F, 0.F, 1.F * 3 / 4});
  ne::RejectNewItem(ImColor{1.F, 1.F / 2, 1.F / 2, alpha}, 4.F);
}
}  // namespace

// ---
void NewLink::Draw(const AppState& app_state) {
  const auto popup_position = ImGui::GetMousePos();
  const auto alpha = GetAlphaForNewLink(app_state.project.GetDiagram());

  {
    const auto create_scope = cpp::Scope{[]() { ne::EndCreate(); }};

    if (ne::BeginCreate(ImColor{1.F, 1.F, 1.F, alpha}, 3.F)) {
      std::cout << "BeginCreate ";
      auto dragged_from_pin = ne::PinId{};
      auto hovering_over_pin = ne::PinId{};

      if (ne::QueryNewLink(&dragged_from_pin, &hovering_over_pin)) {
        std::cout << "QueryNewLink ";
        dragged_from_pin_ = dragged_from_pin;
        hovering_over_pin_ = hovering_over_pin;

        const auto& [can_connect, reason] = GetCanConnectToPinReason(
            *hovering_over_pin_, app_state.project.GetDiagram());

        if (can_connect) {
          if (!reason.empty()) {
            AcceptNewLink(app_state.project.GetDiagram(), app_state.event_queue,
                          reason, alpha);
          }
        } else {
          RejectNewLink(reason, alpha);
        }
      } else if (ne::QueryNewNode(&dragged_from_pin)) {
        std::cout << "QueryNewNode ";
        dragged_from_pin_ = dragged_from_pin;
        hovering_over_pin_.reset();

        DrawTooltip("Create Node", ImColor{0.F, 1.F / 3, 0.F, 1.F * 3 / 4});

        if (ne::AcceptNewItem()) {
          const auto suspend_scope =
              cpp::Scope{[]() { ne::Suspend(); }, []() { ne::Resume(); }};

          app_state.widgets.background_popup.SetPosition(popup_position);
          app_state.widgets.background_popup.Show();
        }
      } else {
        std::cout << "neither ";
        hovering_over_pin_.reset();
      }
    } else {
      std::cout << "false ";
      dragged_from_pin_.reset();
      hovering_over_pin_.reset();
    }

    std::cout << "\n";
  }
}

// ---
auto NewLink::GetDraggedFromPin() const -> const std::optional<ne::PinId>& {
  return dragged_from_pin_;
}

// ---
auto NewLink::GetHoveringOverPin() const -> const std::optional<ne::PinId>& {
  return hovering_over_pin_;
}

// ---
auto NewLink::FindFixedPin(const core::Diagram& diagram) const
    -> std::optional<ne::PinId> {
  if (!dragged_from_pin_.has_value()) {
    return std::nullopt;
  }

  const auto link_to_repin = FindPinLink(diagram, *dragged_from_pin_);

  if (!link_to_repin.has_value()) {
    return std::nullopt;
  }

  if (*dragged_from_pin_ == (*link_to_repin)->start_pin_id) {
    return (*link_to_repin)->end_pin_id;
  }

  return (*link_to_repin)->start_pin_id;
}

// ---
auto NewLink::CanConnectToPin(ne::PinId pin_id,
                              const core::Diagram& diagram) const -> bool {
  return GetCanConnectToPinReason(pin_id, diagram).first;
}

auto NewLink::GetCanConnectToPinReason(ne::PinId pin_id,
                                       const core::Diagram& diagram) const
    -> std::pair<bool, std::string> {
  Expects(dragged_from_pin_.has_value());

  auto rebinding_link = std::optional<RebindingLink>{};
  auto hovered_over_link = std::optional<HoveredOverLink>{};

  for (const auto& link : diagram.GetLinks()) {
    if (link.start_pin_id == *dragged_from_pin_) {
      rebinding_link.emplace(RebindingLink{.fixed_pin = link.end_pin_id,
                                           .fixed_pin_kind = ne::PinKind::Input,
                                           .link_id = link.id});
      break;
    }

    if (link.end_pin_id == *dragged_from_pin_) {
      rebinding_link.emplace(
          RebindingLink{.fixed_pin = link.start_pin_id,
                        .fixed_pin_kind = ne::PinKind::Output,
                        .link_id = link.id});
      break;
    }
  }

  if (hovering_over_pin_.has_value()) {
    for (const auto& link : diagram.GetLinks()) {
      if (link.start_pin_id == *hovering_over_pin_) {
        hovered_over_link.emplace(
            HoveredOverLink{.fixed_pin = link.end_pin_id,
                            .fixed_pin_kind = ne::PinKind::Input,
                            .link_id = link.id});
        break;
      }

      if (link.end_pin_id == *hovering_over_pin_) {
        hovered_over_link.emplace(
            HoveredOverLink{.fixed_pin = link.start_pin_id,
                            .fixed_pin_kind = ne::PinKind::Output,
                            .link_id = link.id});
        break;
      }
    }
  }

  if (hovered_over_link.has_value()) {
    if (rebinding_link.has_value() &&
        (hovered_over_link->link_id == rebinding_link->link_id)) {
      if (pin_id == *hovering_over_pin_) {
        return {true, {}};
      }
    }

    return {false, "Pin Is Occupied"};
  }

  const auto& dragged_from_node = FindPinNode(diagram, *dragged_from_pin_);
  const auto& hovered_over_node = FindPinNode(diagram, pin_id);

  if (dragged_from_node->GetId() == hovered_over_node->GetId()) {
    return {false, "Self"};
  }

  const auto dragged_from_kind =
      GetPinKind(*dragged_from_node, *dragged_from_pin_);
  const auto hovered_over_kind = GetPinKind(*hovered_over_node, pin_id);

  if (rebinding_link.has_value()) {
    if (dragged_from_kind != hovered_over_kind) {
      return {false, "Wrong Pin Kind"};
    }
  } else {
    if (dragged_from_kind == hovered_over_kind) {
      return {false, "Wrong Pin Kind"};
    }
  }

  if (rebinding_link.has_value()) {
    return {true, "Move Link"};
  }

  return {true, "Create Link"};
}

// ---
void NewLink::AcceptNewLink(const core::Diagram& diagram,
                            EventQueue& event_queue, std::string_view tooltip,
                            float alpha) {
  Expects(dragged_from_pin_.has_value());
  Expects(hovering_over_pin_.has_value());

  DrawTooltip(tooltip.data(), ImColor{0.F, 1.F / 3, 0.F, 1.F * 3 / 4});

  if (ne::AcceptNewItem(ImColor{1.F / 2, 1.F, 1.F / 2, alpha}, 4.F)) {
    const auto hovering_over_node = FindPinNode(diagram, *hovering_over_pin_);
    const auto hovering_over_pin_kind =
        core::GetPinKind(*hovering_over_node, *hovering_over_pin_);

    if (const auto fixed_pin = FindFixedPin(diagram)) {
      const auto link_to_repin = core::FindPinLink(diagram, *fixed_pin);
      Expects(link_to_repin.has_value());

      event_queue.PostEvent(
          Events::DeleteLink{.link_id = (*link_to_repin)->id});

      const auto event =
          (hovering_over_pin_kind == ne::PinKind::Input)
              ? Events::CreateLink{.start_pin_id = *fixed_pin,
                                   .end_pin_id = *hovering_over_pin_}
              : Events::CreateLink{.start_pin_id = *hovering_over_pin_,
                                   .end_pin_id = *fixed_pin};
      event_queue.PostEvent(event);
    } else {
      const auto event =
          (hovering_over_pin_kind == ne::PinKind::Input)
              ? Events::CreateLink{.start_pin_id = *dragged_from_pin_,
                                   .end_pin_id = *hovering_over_pin_}
              : Events::CreateLink{.start_pin_id = *hovering_over_pin_,
                                   .end_pin_id = *dragged_from_pin_};
      event_queue.PostEvent(event);
    }
  }
}

auto NewLink::GetAlphaForNewLink(const core::Diagram& diagram) const -> float {
  if (FindFixedPin(diagram).has_value()) {
    return 0.F;
  }

  return 1.F;
}
}  // namespace esc::draw