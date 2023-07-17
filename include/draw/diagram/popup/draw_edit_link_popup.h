/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_DRAW_EDIT_LINK_POPUP_H_
#define VH_PONC_DRAW_EDIT_LINK_POPUP_H_

#include <imgui_node_editor.h>

#include <vector>

#include "core_connection.h"
#include "core_link.h"
#include "coreui_diagram.h"
#include "draw_i_popup.h"

namespace vh::ponc::draw {
///
class EditLinkPopup : public IPopup {
 public:
  ///
  void Draw(coreui::Diagram &diagram,
            const std::vector<core::Connection> &connections);
  ///
  void SetLinkIds(std::vector<ne::LinkId> link_ids);

 private:
  ///
  auto FindLinks(core::Diagram &diagram) const;
  ///
  void CopyLinksAndConnections(
      const std::vector<core::Link *> &links,
      const std::vector<core::Connection> &connections);
  ///
  void SetSelectedConnection(
      const std::vector<core::Link *> &links,
      const std::vector<core::Connection> &connections) const;
  ///
  void Cancel(core::Diagram &diagram) const;

  ///
  std::vector<ne::LinkId> link_ids_{};
  ///
  std::vector<core::Link> link_copies_{};
  ///
  std::vector<const char *> connection_names_{};
  ///
  int connection_name_index_{};

  core::CustomConnection edited_connection_{};
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_EDIT_LINK_POPUP_H_
