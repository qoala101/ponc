#include "draw_new_link.h"

#include "app_events.h"
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
void RejectNewLink(std::string_view tooltip) {
  DrawTooltip(tooltip, ImColor{255 / 3, 0, 0, 255 * 3 / 4});
  ne::RejectNewItem(ImColor{255, 255 / 2, 255 / 2}, 4.F);
}
}  // namespace

// ---
void NewLink::Draw(const AppState& app_state) {
  const auto popup_position = ImGui::GetMousePos();

  {
    const auto create_scope = cpp::Scope{[]() { ne::EndCreate(); }};

    if (ne::BeginCreate(ImColor{1.F, 1.F, 1.F}, 2.F)) {
      dragged_from_pin_.emplace();
      hovering_over_pin_.emplace();

      if (ne::QueryNewLink(&*dragged_from_pin_, &*hovering_over_pin_)) {
        const auto& [can_connect, reason] = GetCanConnectToPinReason(
            app_state.project->GetDiagram(), *hovering_over_pin_);

        if (can_connect) {
          if (!reason.empty()) {
            AcceptNewLink(app_state, reason);
          }
        } else {
          RejectNewLink(reason);
        }
      } else if (ne::QueryNewNode(&*dragged_from_pin_)) {
        hovering_over_pin_.reset();

        DrawTooltip("Create Node", ImColor{0.F, 1.F / 3, 0.F, 1.F * 3 / 4});

        if (ne::AcceptNewItem()) {
          const auto suspend_scope =
              cpp::Scope{[]() { ne::Suspend(); }, []() { ne::Resume(); }};

          app_state.widgets->background_popup.SetPosition(popup_position);
          app_state.widgets->background_popup.Show();
        }
      } else {
        dragged_from_pin_.reset();
        hovering_over_pin_.reset();
      }
    } else {
      dragged_from_pin_.reset();
      hovering_over_pin_.reset();
    }
  }
}

// ---
auto NewLink::IsVisible() const -> bool {
  return dragged_from_pin_.has_value();
}

// ---
auto NewLink::CanConnectToPin(const AppState& app_state, ne::PinId pin_id) const
    -> bool {
  return GetCanConnectToPinReason(app_state.project->GetDiagram(), pin_id)
      .first;
}

// ---
auto NewLink::GetDraggedFromPin() const -> ne::PinId {
  Expects(dragged_from_pin_.has_value());
  return *dragged_from_pin_;
}

auto NewLink::GetCanConnectToPinReason(const core::Diagram& diagram,
                                       ne::PinId pin_id) const
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
void NewLink::AcceptNewLink(const AppState& app_state,
                            std::string_view tooltip) {
  Expects(dragged_from_pin_.has_value());
  Expects(hovering_over_pin_.has_value());

  DrawTooltip(tooltip.data(), ImColor{0.F, 1.F / 3, 0.F, 1.F * 3 / 4});

  if (ne::AcceptNewItem(ImColor{1.F / 2, 1.F, 1.F / 2}, 4.F)) {
    app_state.event_queue->PostEvent(Events::CreateLink{
        .start_pin_id = *dragged_from_pin_, .end_pin_id = *hovering_over_pin_});
  }
}
}  // namespace esc::draw