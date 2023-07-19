/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_DRAW_LINK_POPUP_H_
#define VH_PONC_DRAW_LINK_POPUP_H_

#include <imgui_node_editor.h>

#include <vector>

#include "cpp_callbacks.h"
#include "draw_i_popup.h"

namespace ne = ax::NodeEditor;

namespace vh::ponc::draw {
///
class LinkPopup : public IPopup {
 public:
  ///
  struct Callbacks {
    ///
    cpp::Signal<std::vector<ne::LinkId>> delete_selected{};
  };

  ///
  void Draw(const Callbacks &callbacks);
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_LINK_POPUP_H_
