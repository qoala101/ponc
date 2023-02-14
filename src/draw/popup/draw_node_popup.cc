#include "draw_node_popup.h"

#include <iostream>

#include "cpp_scope.h"
#include "draw_i_node_drawer.h"
#include "imgui.h"

namespace esc::draw {
NodePopup::NodePopup(std::shared_ptr<core::INode> node)
    : node_{std::move(node)} {}

auto NodePopup::GetLabel(State& state) const -> std::string {
  return node_->CreateDrawer(state)->GetLabel();
}

void NodePopup::DrawContent(State& state) {
  if (ImGui::MenuItem("Delete")) {
    SetVisible(false);

    state.PostEvent([node_id = node_->GetId()](auto& state) {
      State::EraseNodeAndConnectedLinks(state, node_id);
    });
  }

  if (ImGui::MenuItem("Delete Ex")) {
    SetVisible(false);

    state.PostEvent([node_id = node_->GetId()](auto& state) {
      State::ReplaceWithPlaceholder(state, node_id);
    });
  }

  if (ImGui::BeginMenu("Group")) {
    const auto menu_scope = cpp::Scope{[]() { ImGui::EndMenu(); }};

    ImGui::InputTextWithHint("", "Enter group name...", group_name_.data(),
                             group_name_.size());
    ImGui::SameLine();

    if (ImGui::SmallButton("Add")) {
      SetVisible(false);

      state.PostEvent(
          [group_name = std::string{group_name_.begin(), group_name_.end()}](
              auto& state) {
            State::MakeGroupFromSelectedNodes(state, group_name);
          });
    }
  }
}
}  // namespace esc::draw