#include "draw_node_editor.h"

#include "cpp_scope.h"
#include "draw_links.h"
#include "draw_main_window.h"
#include "draw_nodes.h"

namespace esc::draw {
NodeEditor::NodeEditor() : editor_context_{ne::CreateEditor()} {
  ne::SetCurrentEditor(editor_context_);
}

NodeEditor::~NodeEditor() { ne::DestroyEditor(editor_context_); }

void NodeEditor::Draw(coreui::Frame &frame) {
  const auto node_editor_scope =
      cpp::Scope{[]() { ne::Begin("Node editor"); }, []() { ne::End(); }};

  // draw::DrawGroups(state);
  creation.Draw(frame);
  nodes.Draw(frame);
  DrawLinks(frame.links, frame.curve);
  // draw::DrawDeleteItemsProcess(state);
  DrawShowPopupProcess(frame);
  DrawPopupContents(frame);
}

auto NodeEditor::GetCreation() const -> const Creation & { return creation; }

auto NodeEditor::GetNodes() const -> const Nodes & { return nodes; }

void NodeEditor::DrawShowPopupProcess(coreui::Frame &frame) {
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

void NodeEditor::DrawPopupContents(coreui::Frame &frame) {
  const auto suspend_scope =
      cpp::Scope{[]() { ne::Suspend(); }, []() { ne::Resume(); }};

  background_popup.Draw(frame);
  node_popup.Draw(frame);
  link_popup.Draw(frame);
}
}  // namespace esc::draw