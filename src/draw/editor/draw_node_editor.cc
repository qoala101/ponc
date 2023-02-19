#include "draw_node_editor.h"

#include "cpp_scope.h"

namespace esc::draw {
NodeEditor::NodeEditor() : editor_context_{ne::CreateEditor()} {
  ne::SetCurrentEditor(editor_context_);
}

NodeEditor::~NodeEditor() { ne::DestroyEditor(editor_context_); }

void NodeEditor::Draw() {
  const auto node_editor_scope =
      cpp::Scope{[]() { ne::Begin("Node editor"); }, []() { ne::End(); }};

  // widgets.link_being_repinned.Draw(state);
  // draw::DrawGroups(state);
  // widgets.links.Draw(state);
  // widgets.nodes.Draw(state);
  // widgets.link_connection_process.Draw(state);
  // draw::DrawDeleteItemsProcess(state);
  // draw::DrawPopups(state);
}
}  // namespace esc::draw