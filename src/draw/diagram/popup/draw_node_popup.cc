/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "draw_node_popup.h"

#include <imgui.h>
#include <imgui_node_editor.h>

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

#include "core_diagram.h"
#include "core_i_node.h"
#include "core_tag.h"
#include "coreui_family.h"
#include "coreui_i_node_traits.h"
#include "coreui_native_facade.h"
#include "coreui_tags.h"
#include "cpp_callbacks.h"
#include "draw_disable_if.h"
#include "draw_family_groups_menu.h"

namespace vh::ponc::draw {
namespace {
///
auto GetTitle(const std::vector<ne::NodeId>& nodes) {
  if (nodes.empty()) {
    return std::string{};
  }

  if (nodes.size() == 1) {
    return "Node #" + std::to_string(nodes.front().Get());
  }

  return "Nodes (" + std::to_string(nodes.size()) + ")";
}

///
void DrawReplaceActions(coreui::Diagram& diagram, const core::INode& node) {
  if (ImGui::BeginMenu("Replace With")) {
    FamilyGroupsMenu::Draw(
        diagram.GetFamilyGroups(),
        {.is_family_enabled =
             [&diagram, &node](const auto& family) {
               const auto sample_node = family.CreateSampleNode();
               return diagram.CanReplaceNode(node, *sample_node);
             },
         .family_selected =
             [&diagram, &node](const auto& family) {
               diagram.ReplaceNode(node, family.CreateNode());
             }});

    ImGui::EndMenu();
  }
}

///
void DrawNodeActions(core::INode& node) {
  const auto node_traits = node.CreateUiTraits();
  const auto node_action_names = node_traits->GetActionNames();

  if (node_action_names.empty()) {
    return;
  }

  ImGui::Separator();

  for (const auto& action_name : node_action_names) {
    if (ImGui::MenuItem(action_name.c_str())) {
      node_traits->ExecuteAction(action_name);
    }
  }
}
}  // namespace

///
void NodePopup::Draw(coreui::Diagram& diagram, coreui::Tags& tags) {
  const auto selected_nodes = IsOpened()
                                  ? coreui::NativeFacade::GetSelectedNodes()
                                  : std::vector<ne::NodeId>{};
  const auto title = GetTitle(selected_nodes);
  const auto content_scope = DrawContentScope(title);

  if (!IsOpened()) {
    return;
  }

  if (ImGui::MenuItem("Delete")) {
    for (const auto node_id : selected_nodes) {
      diagram.DeleteNode(node_id);
    }
  }

  if (ImGui::MenuItem("Delete With Links")) {
    for (const auto node_id : selected_nodes) {
      diagram.DeleteNodeWithLinks(node_id);
    }
  }

  ImGui::Separator();
  DrawTagActions(tags, selected_nodes);
  ImGui::Separator();

  if (ImGui::MenuItem("Tree Select")) {
    diagram.TreeSelect(selected_nodes);
  }

  if (ImGui::MenuItem("Tree Arrange")) {
    diagram.TreeArrange(selected_nodes);
  }

  if (selected_nodes.size() > 1) {
    return;
  }

  auto& node =
      core::Diagram::FindNode(diagram.GetDiagram(), selected_nodes.front());

  ImGui::Separator();
  DrawReplaceActions(diagram, node);
  DrawNodeActions(node);
}

///
void NodePopup::OnOpen() {
  tag_name_buffer_.Clear();
  tag_checks_.clear();
}

///
auto NodePopup::CanAddTag(const core::Tags& tags) const {
  const auto new_tag_name = tag_name_buffer_.AsTrimmed();

  return !new_tag_name.empty() &&
         std::all_of(tags.cbegin(), tags.cend(),
                     [&new_tag_name](const auto& tag) {
                       return tag->name != new_tag_name;
                     });
}

///
void NodePopup::AddTag(const coreui::Tags& tags,
                       const std::vector<ne::NodeId>& selected_nodes) {
  auto new_tag = std::make_shared<core::Tag>();
  new_tag->name = tag_name_buffer_.AsTrimmed();

  tags.SetNextTagState(new_tag, selected_nodes);
  tags.AddTag(std::move(new_tag));

  tag_name_buffer_.Clear();
}

///
void NodePopup::DrawAddTag(coreui::Tags& tags,
                           const std::vector<ne::NodeId>& selected_nodes) {
  ImGui::InputText("##Tag Name", tag_name_buffer_.GetData(),
                   tag_name_buffer_.GetSize());
  ImGui::SameLine();

  {
    const auto disable_scope = EnableIf(CanAddTag(tags.GetTags()));

    if (ImGui::Button("Add")) {
      AddTag(tags, selected_nodes);
    }
  }
}

///
void NodePopup::DrawTagChecks(coreui::Tags& tags,
                              const std::vector<ne::NodeId>& selected_nodes) {
  const auto& core_tags = tags.GetTags();
  tag_checks_.resize(core_tags.size());

  if (ImGui::BeginListBox("")) {
    for (auto i = 0; i < static_cast<int>(core_tags.size()); ++i) {
      const auto& tag = core_tags[i];
      const auto tag_state = tags.GetTagState(tag->name, selected_nodes);

      auto& tag_check = At(tag_checks_, i);
      tag_check = tag_state == coreui::TagState::kChecked;

      ImGui::PushItemFlag(ImGuiItemFlags_MixedValue,
                          tag_state == coreui::TagState::kIntermediate);

      if (ImGui::Checkbox(tag->name.c_str(), &tag_check)) {
        tags.SetNextTagState(tag, selected_nodes);
      }

      ImGui::PopItemFlag();
    }

    ImGui::EndListBox();
  }
}

///
void NodePopup::DrawTagActions(coreui::Tags& tags,
                               const std::vector<ne::NodeId>& selected_nodes) {
  if (ImGui::BeginMenu("Tags")) {
    DrawAddTag(tags, selected_nodes);
    DrawTagChecks(tags, selected_nodes);
    ImGui::EndMenu();
  }
}
}  // namespace vh::ponc::draw