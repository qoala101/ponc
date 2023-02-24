#include "draw_node_editor.h"

#include "cpp_scope.h"
#include "draw_links.h"
#include "draw_popups.h"
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
  app_state.widgets.new_link.Draw(app_state);
  app_state.widgets.nodes.Draw(app_state.frame);
  app_state.widgets.links.Draw(app_state.frame);
  // draw::DrawDeleteItemsProcess(state);
  draw::DrawPopups(app_state);
}
}  // namespace esc::draw