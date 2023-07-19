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
#include "core_project.h"
#include "coreui_diagram.h"
#include "draw_i_popup.h"

namespace vh::ponc::draw {
///
class EditLinkPopup : public IPopup {
 public:
  ///
  void Draw(coreui::Diagram &diagram, const core::Project &project);
  ///
  void SetLinkId(ne::LinkId link_id);

 private:
  ///
  void CopyConnections(const std::vector<core::Connection> &connections);
  ///
  auto IsDefaultConnection() const;
  ///
  void SetSelectedConnection(core::Link &link,
                             const std::vector<core::Connection> &connections);

  ///
  ne::LinkId link_id_{};
  ///
  core::Link link_copy_{};
  ///
  std::vector<const char *> connection_names_{};
  ///
  int connection_index_{};
  ///
  core::CustomConnection custom_connection_copy_{};
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_EDIT_LINK_POPUP_H_
