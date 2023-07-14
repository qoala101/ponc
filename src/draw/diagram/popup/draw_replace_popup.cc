/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_replace_popup.h"

#include <imgui.h>
#include <imgui_node_editor.h>

#include <algorithm>
#include <memory>
#include <regex>
#include <string>
#include <utility>
#include <vector>

#include "core_diagram.h"
#include "core_i_node.h"
#include "core_id_value.h"
#include "coreui_cloner.h"
#include "coreui_family.h"
#include "coreui_node_replacer.h"
#include "cpp_assert.h"

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
    CopyNodeAndValidFamilies(diagram);
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
    Replace(diagram);
  }

  ImGui::SameLine();

  if (ImGui::Button("Cancel")) {
    Cancel(diagram);
    ImGui::CloseCurrentPopup();
  }
}

///
void ReplacePopup::SetNodeId(ne::NodeId node_id) { node_id_ = node_id; }

///
void ReplacePopup::OnOpen() const {
  ImGui::SetNextWindowPos(ImGui::GetMousePos(), ImGuiCond_Appearing,
                          ImVec2{0.5, 1});
}

///
void ReplacePopup::CopyNodeAndValidFamilies(const coreui::Diagram& diagram) {
  const auto& node = core::Diagram::FindNode(diagram.GetDiagram(), node_id_);
  const auto node_family_id = node.GetFamilyId();
  const auto& node_replacer = diagram.GetNodeReplacer();

  families_.clear();
  family_index_ = 0;

  auto family_index = 0;

  for (const auto& family_group : diagram.GetFamilyGroups()) {
    for (const auto& family : family_group.families) {
      const auto sample_node = family.GetFamily().CreateSampleNode();

      if (!node_replacer.CanReplaceNode(node, *sample_node)) {
        continue;
      }

      const auto family_id = family.GetFamily().GetId();

      families_.emplace_back(Family{
          .id = family_id, .label = EscapeFormatCharacters(family.GetLabel())});

      if (family_id == node_family_id) {
        node_copy_ = coreui::Cloner::Clone(node, family.GetFamily());
        family_index_ = family_index;
      }

      ++family_index;
    }
  }
}

///
auto ReplacePopup::FindSelectedFamily(
    const std::vector<coreui::FamilyGroup>& family_groups) const -> auto& {
  Expects(family_index_ >= 0);
  Expects(family_index_ < static_cast<int>(families_.size()));

  const auto family_id = families_[family_index_].id;

  for (const auto& family_group : family_groups) {
    const auto& families = family_group.families;
    const auto family = std::find_if(
        families.cbegin(), families.cend(), [family_id](const auto& family) {
          return family.GetFamily().GetId() == family_id;
        });

    if (family != families.cend()) {
      return family->GetFamily();
    }
  }

  Expects(false);
}

///
void ReplacePopup::Replace(const coreui::Diagram& diagram) const {
  const auto& targe_family = FindSelectedFamily(diagram.GetFamilyGroups());
  const auto same_family = (targe_family.GetId() == node_copy_->GetFamilyId());

  auto new_node = same_family ? coreui::Cloner::Clone(*node_copy_, targe_family)
                              : targe_family.CreateSampleNode();

  const auto& node = core::Diagram::FindNode(diagram.GetDiagram(), node_id_);
  const auto& source_output_pins = node_copy_->GetOutputPinIds();
  auto reusable_ids = core::INode::GetIds(std::as_const(*node_copy_));

  diagram.GetNodeReplacer().ReplaceNode(
      node, source_output_pins, std::move(new_node), std::move(reusable_ids));
}

///
void ReplacePopup::Cancel(const coreui::Diagram& diagram) {
  const auto& node = core::Diagram::FindNode(diagram.GetDiagram(), node_id_);
  const auto source_output_pins = node_copy_->GetOutputPinIds();
  auto reusable_ids = core::INode::GetIds(std::as_const(*node_copy_));

  diagram.GetNodeReplacer().ReplaceNode(
      node, source_output_pins, std::move(node_copy_), std::move(reusable_ids));
}
}  // namespace vh::ponc::draw