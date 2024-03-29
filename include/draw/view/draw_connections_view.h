/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_DRAW_CONNECTIONS_VIEW_H_
#define VH_PONC_DRAW_CONNECTIONS_VIEW_H_

#include <optional>
#include <string>

#include "core_connection.h"
#include "coreui_project.h"
#include "draw_i_view.h"
#include "draw_string_buffer.h"

namespace vh::ponc::draw {
///
class ConnectionsView : public IView {
 public:
  ///
  auto GetLabel() const -> std::string override;

  ///
  void Draw(coreui::Project& project);

 private:
  ///
  enum class Action { kStartRenaming, kConfirmRename, kDelete };

  ///
  auto DrawRenamePopup();
  ///
  auto DrawControls(coreui::Project& project);
  ///
  void ApplyAction(coreui::Project& project, core::Connection& connection,
                   Action action);
  ///
  void DrawDiagrams(coreui::Project& project,
                    std::optional<Action> selected_action);

  ///
  StringBuffer rename_buffer_{};
  ///
  bool focus_name_input_{};
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_CONNECTIONS_VIEW_H_
