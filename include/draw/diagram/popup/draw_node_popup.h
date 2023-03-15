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
    cpp::Signal<const std::vector<ne::NodeId> &> delete_selected{};
    ///
    cpp::Signal<const std::vector<ne::NodeId> &> delete_with_links_selected{};
  };

  ///
  void Draw(const Callbacks &callbacks);
};
}  // namespace esc::draw

#endif  // VH_DRAW_NODE_POPUP_H_
