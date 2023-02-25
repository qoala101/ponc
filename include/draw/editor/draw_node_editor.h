#ifndef VH_DRAW_NODE_EDITOR_H_
#define VH_DRAW_NODE_EDITOR_H_

#include <imgui_node_editor.h>

#include <memory>

#include "draw_background_popup.h"
#include "draw_link_popup.h"
#include "draw_links.h"
#include "draw_new_link.h"
#include "draw_node_popup.h"
#include "draw_nodes.h"
#include "frame_node.h"

namespace ne = ax::NodeEditor;

namespace esc::draw {
class NodeEditor {
 public:
  NodeEditor();

  NodeEditor(const NodeEditor &) = delete;
  NodeEditor(NodeEditor &&) noexcept = delete;

  auto operator=(const NodeEditor &) noexcept -> NodeEditor & = delete;
  auto operator=(NodeEditor &&) noexcept -> NodeEditor & = delete;

  ~NodeEditor();

  void Draw(frame::Frame &frame);

  auto GetNewLink() const -> const NewLink &;

  auto GetNodes() const -> const Nodes &;

 private:
  void DrawShowPopupProcess(frame::Frame &frame);
  void DrawPopupContents(frame::Frame &frame);

  ne::EditorContext *editor_context_{};

  NewLink new_link{};
  Nodes nodes{};

  BackgroundPopup background_popup{};
  NodePopup node_popup{};
  LinkPopup link_popup{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_NODE_EDITOR_H_
