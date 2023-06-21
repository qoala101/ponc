/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_replace_popup.h"

#include <imgui.h>
#include <imgui_node_editor.h>

#include <memory>
#include <regex>
#include <string>
#include <vector>

#include "core_diagram.h"
#include "core_i_node.h"
#include "coreui_family.h"
#include "cpp_assert.h"
#include "draw_disable_if.h"

namespace vh::ponc::draw {
namespace {
///
auto EscapeFormatCharacters(const std::string& text) {
  return std::regex_replace(text, std::regex{"%"}, "%%");
}
}  // namespace

///
void ReplacePopup::Draw(coreui::Diagram& diagram) {
  const auto content_scope = DrawContentScope();

  if (!IsOpened()) {
    return;
  }

  if (WasJustOpened()) {
    OnOpen(diagram);
  }

  if (families_.size() <= 1) {
    ImGui::TextUnformatted("No replace candidates");
    return;
  }

  Expects(family_index_ >= 0);
  Expects(family_index_ < static_cast<int>(families_.size()));

  const auto& family = families_[family_index_];

  if (ImGui::SliderInt("", &family_index_, 0,
                       static_cast<int>(families_.size()) - 1,
                       family.label.c_str(), ImGuiSliderFlags_NoInput)) {
  }

  ImGui::SameLine();

  {
    const auto disable_scope = DisableIf(family.id == node_family_id_);

    if (ImGui::Button("Replace")) {
      Replace(diagram);
      ImGui::CloseCurrentPopup();
    }
  }
}

///
void ReplacePopup::SetNodeId(ne::NodeId node_id) { node_id_ = node_id; }

///
void ReplacePopup::OnOpen(const coreui::Diagram& diagram) {
  const auto& node = core::Diagram::FindNode(diagram.GetDiagram(), node_id_);

  node_family_id_ = node.GetFamilyId();
  families_.clear();
  family_index_ = 0;

  auto family_index = 0;

  for (const auto& family_group : diagram.GetFamilyGroups()) {
    for (const auto& family : family_group.families) {
      const auto sample_node = family.GetFamily().CreateSampleNode();

      if (!diagram.CanReplaceNode(node, *sample_node)) {
        continue;
      }

      const auto family_id = family.GetFamily().GetId();

      families_.emplace_back(Family{
          .id = family_id, .label = EscapeFormatCharacters(family.GetLabel())});

      if (family_id == node_family_id_) {
        family_index_ = family_index;
      }

      ++family_index;
    }
  }
}

///
void ReplacePopup::Replace(const coreui::Diagram& diagram) const {
  const auto& node = core::Diagram::FindNode(diagram.GetDiagram(), node_id_);

  Expects(family_index_ >= 0);
  Expects(family_index_ < static_cast<int>(families_.size()));

  const auto family_id = families_[family_index_].id;

  for (const auto& family_group : diagram.GetFamilyGroups()) {
    const auto& families = family_group.families;
    const auto family = std::find_if(
        families.cbegin(), families.cend(), [family_id](const auto& family) {
          return family.GetFamily().GetId() == family_id;
        });

    if (family == families.cend()) {
      continue;
    }

    diagram.ReplaceNode(node, family->CreateNode());
    return;
  }
}
}  // namespace vh::ponc::draw