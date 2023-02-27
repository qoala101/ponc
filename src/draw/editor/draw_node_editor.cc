#include "draw_node_editor.h"

#include <functional>

#include "cpp_scope.h"
#include "draw_links.h"
#include "draw_main_window.h"
#include "draw_nodes.h"
#include "imgui_node_editor.h"

namespace esc::draw {
NodeEditor::NodeEditor() : editor_context_{ne::CreateEditor()} {
  ne::SetCurrentEditor(editor_context_);
}

NodeEditor::~NodeEditor() { ne::DestroyEditor(editor_context_); }

void NodeEditor::Draw(coreui::Frame &frame) {
  ne::Begin("Node editor");

  // draw::DrawGroups(state);
  creation.Draw(frame.creation, frame.new_link,
                std::bind_front(&NodeEditor::SlotCreateCurrentLink, frame),
                std::bind_front(&NodeEditor::SlotCreateConnectedNode, this));
  DrawNodes(frame.nodes);
  DrawLinks(frame.links);
  // draw::DrawDeleteItemsProcess(state);
  DrawShowPopupProcess(frame);
  DrawPopupContents(frame);

  ne::End();
}

void NodeEditor::DrawShowPopupProcess(coreui::Frame &frame) {
  const auto popup_position = ImGui::GetMousePos();

  ne::Suspend();
  const auto resume_scope = cpp::Scope{[]() { ne::Resume(); }};

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

void NodeEditor::DrawPopupContents(coreui::Frame &frame) {
  background_popup.Draw(frame.GetProject().GetFamilies(),
                        std::bind_front(&NodeEditor::SlotCreateNode, frame));
  node_popup.Draw();
  link_popup.Draw(std::bind_front(&NodeEditor::SlotDeleteLink, frame));
  creation_popup.Draw(frame);
}

void NodeEditor::SlotCreateCurrentLink(coreui::Frame &frame) {}

void NodeEditor::SlotCreateConnectedNode(const ImVec2 &new_node_pos,
                                         ne::PinId connect_to_pin) {
  ne::Suspend();

  creation_popup.SetPosition(new_node_pos);
  creation_popup.SetDraggedFromPin(connect_to_pin);
  creation_popup.Show();

  ne::Resume();
}

void NodeEditor::SlotCreateNode(coreui::Frame &frame,
                                const std::shared_ptr<core::IFamily> &family,
                                const ImVec2 &pos) {
  frame.EmplaceNode(family->CreateNode(frame.GetProject().GetIdGenerator()),
                    pos);
}

void NodeEditor::SlotDeleteLink(coreui::Frame &frame, ne::LinkId link_id) {
  frame.DeleteLink(link_id);
}
}  // namespace esc::draw