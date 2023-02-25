#include "draw_node_editor.h"

#include "cpp_scope.h"
#include "draw_links.h"
#include "draw_nodes.h"
#include "draw_widgets.h"

namespace esc::draw {
NodeEditor::NodeEditor() : editor_context_{ne::CreateEditor()} {
  ne::SetCurrentEditor(editor_context_);
}

NodeEditor::~NodeEditor() { ne::DestroyEditor(editor_context_); }

void NodeEditor::Draw(const AppState &app_state) {
  const auto node_editor_scope =
      cpp::Scope{[]() { ne::Begin("Node editor"); }, []() { ne::End(); }};

  // draw::DrawGroups(state);
  new_link.Draw(app_state.frame);
  nodes.Draw(app_state.frame);
  DrawLinks(app_state.frame);
  // draw::DrawDeleteItemsProcess(state);
  DrawShowPopupProcess(app_state);
  DrawPopupContents(app_state);
}

auto NodeEditor::GetNewLink() const -> const NewLink & { return new_link; }

auto NodeEditor::GetNodes() const -> const Nodes & { return nodes; }

void NodeEditor::DrawShowPopupProcess(const AppState &app_state) {
  const auto popup_position = ImGui::GetMousePos();

  {
    const auto suspend_scope =
        cpp::Scope{[]() { ne::Suspend(); }, []() { ne::Resume(); }};

    if (ne::ShowBackgroundContextMenu()) {
      background_popup.SetPosition(popup_position);
      background_popup.Show();
      return;
    }

    auto popup_node_id = ne::NodeId{};

    if (ne::ShowNodeContextMenu(&popup_node_id)) {
      node_popup.SetNodeId(popup_node_id);
      node_popup.Show();
      return;
    }

    auto popup_link_id = ne::LinkId{};

    if (ne::ShowLinkContextMenu(&popup_link_id)) {
      link_popup.SetLinkId(popup_link_id);
      link_popup.Show();
      return;
    }
  }
}

void NodeEditor::DrawPopupContents(const AppState &app_state) {
  const auto suspend_scope =
      cpp::Scope{[]() { ne::Suspend(); }, []() { ne::Resume(); }};

  background_popup.Draw(app_state.frame);
  node_popup.Draw(app_state.frame);
  link_popup.Draw(app_state.frame);
}
}  // namespace esc::draw