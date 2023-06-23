/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_DRAW_REPLACE_POPUP_H_
#define VH_PONC_DRAW_REPLACE_POPUP_H_

#include <imgui_node_editor.h>

#include <memory>

#include "core_i_family.h"
#include "coreui_diagram.h"
#include "draw_i_popup.h"

namespace vh::ponc::draw {
///
class ReplacePopup : public IPopup {
 public:
  ///
  void Draw(coreui::Diagram &diagram);
  ///
  void SetNodeId(ne::NodeId node_id);

 private:
  ///
  struct Family {
    ///
    core::FamilyId id{};
    ///
    std::string label{};
  };

  ///
  void OnOpen() const override;

  ///
  void OnOpen(const coreui::Diagram &diagram);
  ///
  auto FindSelectedFamily(
      const std::vector<coreui::FamilyGroup> &family_groups) const -> auto &;
  ///
  void Replace(const coreui::Diagram &diagram) const;
  ///
  void Cancel(const coreui::Diagram &diagram);

  ///
  ne::NodeId node_id_{};
  ///
  std::unique_ptr<core::INode> node_copy_{};
  ///
  std::vector<Family> families_{};
  ///
  int family_index_{};
};
}  // namespace vh::ponc::draw

#endif  // VH_PONC_DRAW_REPLACE_POPUP_H_
