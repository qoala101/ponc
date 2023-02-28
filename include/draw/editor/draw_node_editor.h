#ifndef VH_DRAW_NODE_EDITOR_H_
#define VH_DRAW_NODE_EDITOR_H_

#include <imgui_node_editor.h>

#include <memory>

#include "draw_background_popup.h"
#include "draw_creation_popup.h"
#include "draw_link_popup.h"
#include "draw_node_popup.h"
#include "frame_node.h"

namespace ne = ax::NodeEditor;

namespace esc::draw {
class NodeEditor {
 public:
  NodeEditor();

  NodeEditor(const NodeEditor &) = delete;
  NodeEditor(NodeEditor &&) noexcept = delete;

  auto operator=(const NodeEditor &) -> NodeEditor & = delete;
  auto operator=(NodeEditor &&) noexcept -> NodeEditor & = delete;

  ~NodeEditor();

  void Draw(coreui::Frame &frame);

 private:
  static void SlotCreateNode(coreui::Frame &frame,
                             const std::shared_ptr<core::IFamily> &family,
                             const ImVec2 &pos);
  static void SlotDeleteLink(coreui::Frame &frame, ne::LinkId link_id);
  static void SlotCreateCurrentLink(coreui::Frame &frame);

  void DrawShowPopupProcess(coreui::Frame &frame);
  void DrawPopupContents(coreui::Frame &frame);

  void SlotCreateConnectedNode(const ImVec2 &new_node_pos,
                               ne::PinId connect_to_pin);

  ne::EditorContext *editor_context_{};

  BackgroundPopup background_popup{};
  CreationPopup creation_popup{};
  NodePopup node_popup{};
  LinkPopup link_popup{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_NODE_EDITOR_H_
