/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_DRAW_DIAGRAMS_VIEW_H_
#define VH_PONC_DRAW_DIAGRAMS_VIEW_H_

#include <string>

#include "coreui_project.h"
#include "draw_i_view.h"

namespace vh::ponc::draw {
///
class DiagramsView : public IView {
 public:
  ///
  auto GetLabel() const -> std::string override;

  ///
  void Draw(coreui::Project& project);

 private:
  ///
  enum class Action { kClone, kStartRenaming, kConfirmRename, kDelete };

  ///
  auto GetTrimmedRenameBuffer() const;
  ///
  auto DrawRenamePopup();
  ///
  auto DrawControls(coreui::Project& project);
  ///
  void ApplyAction(coreui::Project& project, int diagram_index, Action action);
  ///
  void DrawDiagrams(coreui::Project& project,
                    std::optional<Action> selected_action);

  ///
  std::string rename_buffer_{};
  ///
  bool focus_name_input_{};
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_DIAGRAMS_VIEW_H_
