#ifndef VH_DRAW_NODE_EDITOR_H_
#define VH_DRAW_NODE_EDITOR_H_

#include <imgui_node_editor.h>

#include <memory>

#include "app_state.h"

namespace ne = ax::NodeEditor;

namespace esc::draw {
// ---
class NodeEditor {
 public:
  // ---
  NodeEditor();

  // ---
  NodeEditor(const NodeEditor &) = delete;
  // ---
  NodeEditor(NodeEditor &&) noexcept = delete;

  // ---
  auto operator=(const NodeEditor &) noexcept -> NodeEditor & = delete;
  // ---
  auto operator=(NodeEditor &&) noexcept -> NodeEditor & = delete;

  // ---
  ~NodeEditor();

  // ---
  void Draw(const AppState &app_state);

 private:
  // ---
  ne::EditorContext *editor_context_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_NODE_EDITOR_H_
