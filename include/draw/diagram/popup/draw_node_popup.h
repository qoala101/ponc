/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_DRAW_NODE_POPUP_H_
#define VH_PONC_DRAW_NODE_POPUP_H_

#include <imgui_node_editor.h>

#include "core_i_node.h"
#include "coreui_diagram.h"
#include "coreui_tags.h"
#include "cpp_vector_bool.h"
#include "draw_i_popup.h"
#include "draw_string_buffer.h"

namespace vh::ponc::draw {
///
class NodePopup : public IPopup {
 public:
  ///
  void Draw(coreui::Diagram &diagram, coreui::Tags &tags);

 private:
  ///
  void OnOpen() override;
  ///
  auto CanAddTag(const core::Tags &tags) const;
  ///
  void AddTag(const coreui::Tags &tags,
              const std::vector<ne::NodeId> &selected_nodes);
  ///
  void DrawAddTag(coreui::Tags &tags,
                  const std::vector<ne::NodeId> &selected_nodes);
  ///
  void DrawTagChecks(coreui::Tags &tags,
                     const std::vector<ne::NodeId> &selected_nodes);
  ///
  void DrawTagActions(coreui::Tags &tags,
                      const std::vector<ne::NodeId> &selected_nodes);

  ///
  StringBuffer tag_name_buffer_{};
  ///
  cpp::VectorBool tag_checks_{};
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_NODE_POPUP_H_
