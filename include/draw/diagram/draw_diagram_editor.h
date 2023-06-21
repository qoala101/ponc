/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_DRAW_DIAGRAM_EDITOR_H_
#define VH_PONC_DRAW_DIAGRAM_EDITOR_H_

#include <imgui_node_editor.h>

#include <memory>

#include "core_diagram.h"
#include "coreui_diagram.h"
#include "draw_background_popup.h"
#include "draw_item_deleter.h"
#include "draw_link_popup.h"
#include "draw_linker.h"
#include "draw_node_popup.h"
#include "draw_replace_popup.h"

namespace vh::ponc::draw {
///
class DiagramEditor {
 public:
  ///
  DiagramEditor();

  ///
  void Draw(coreui::Diagram &diagram);

 private:
  ///
  void OpenPopupsIfRequested(const core::Diagram &diagram);
  ///
  void DrawPopups(coreui::Diagram &diagram);

  ///
  ne::Config config_{};
  ///
  std::unique_ptr<ne::EditorContext, void (*)(ne::EditorContext *)> context_;
  ///
  ItemDeleter item_deleter_{};
  ///
  Linker linker_{};
  ///
  BackgroundPopup background_popup_{};
  ///
  NodePopup node_popup_{};
  ///
  LinkPopup link_popup_{};
  ///
  ReplacePopup replace_popup_{};
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_DIAGRAM_EDITOR_H_
