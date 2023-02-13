#include "draw_node_popup.h"

#include "draw_i_node_drawer.h"

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
}
}  // namespace esc::draw