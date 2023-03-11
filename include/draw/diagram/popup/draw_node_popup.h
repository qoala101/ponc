#ifndef VH_DRAW_NODE_POPUP_H_
#define VH_DRAW_NODE_POPUP_H_

#include <imgui_node_editor.h>

#include "draw_i_popup.h"

namespace ne = ax::NodeEditor;

namespace esc::draw {
///
class NodePopup : public IPopup {
 public:
  ///
  struct Callbacks {
    ///
    cpp::Signal<ne::NodeId> node_deleted{};
    ///
    cpp::Signal<ne::NodeId> node_deleted_with_links{};
  };

  ///
  void Draw(const Callbacks &callbacks);
  ///
  void SetNodeId(ne::NodeId node_id);

 private:
  ///
  auto GetLabel() const -> std::string override;

  ///
  ne::NodeId node_id_{};
};
}  // namespace esc::draw

#endif  // VH_DRAW_NODE_POPUP_H_
