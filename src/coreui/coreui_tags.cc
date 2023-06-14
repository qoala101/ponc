/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "coreui_tags.h"

#include "coreui_project.h"
#include "cpp_assert.h"

namespace vh::ponc::coreui {
///
Tags::Tags(cpp::SafePtr<Project> parent_project)
    : parent_project_{std::move(parent_project)} {}

///
auto Tags::GetTags() const -> const core::Tags& {
  return parent_project_->GetProject().GetTags();
}

///
auto Tags::AddTag(std::shared_ptr<core::Tag> tag) const -> Event& {
  return parent_project_->GetEventLoop().PostEvent(
      [parent_project = parent_project_, tag = std::move(tag)]() mutable {
        parent_project->GetProject().EmplaceTag(std::move(tag));
      });
}

///
auto Tags::DeleteTag(std::string tag_name) const -> Event& {
  return parent_project_->GetEventLoop().PostEvent(
      [parent_project = parent_project_, tag_name = std::move(tag_name)]() {
        parent_project->GetProject().DeleteTag(tag_name);
      });
}

///
auto Tags::GetTagState(std::string_view tag_name,
                       const std::vector<ne::NodeId>& node_ids) const
    -> TagState {
  const auto num_tagged_nodes = std::count_if(
      node_ids.cbegin(), node_ids.cend(),
      [parent_project = parent_project_, &tag_name](const auto node_id) {
        const auto& diagram = parent_project->GetDiagram().GetDiagram();
        const auto& node = core::Diagram::FindNode(diagram, node_id);
        return core::INode::HasTag(node, tag_name);
      });

  if (const auto all_nodes_have_tag =
          num_tagged_nodes == static_cast<int>(node_ids.size())) {
    return TagState::kChecked;
  }

  if (const auto no_nodes_have_tag = num_tagged_nodes <= 0) {
    return TagState::kUnchecked;
  }

  return TagState::kIntermediate;
}

///
auto Tags::SetNextTagState(std::shared_ptr<core::Tag> tag,
                           std::vector<ne::NodeId> node_ids) const -> Event& {
  const auto current_state = GetTagState(tag->name, node_ids);

  switch (current_state) {
    case kUnchecked:
    case kIntermediate:
      return parent_project_->GetEventLoop().PostEvent(
          [parent_project = parent_project_, tag = std::move(tag),
           node_ids = std::move(node_ids)]() {
            const auto& diagram = parent_project->GetDiagram().GetDiagram();

            for (const auto node_id : node_ids) {
              auto& node = core::Diagram::FindNode(diagram, node_id);
              node.AddTag(tag);
            }
          });
    case kChecked:
      return parent_project_->GetEventLoop().PostEvent(
          [parent_project = parent_project_, tag = std::move(tag),
           node_ids = std::move(node_ids)]() {
            const auto& diagram = parent_project->GetDiagram().GetDiagram();

            for (const auto node_id : node_ids) {
              auto& node = core::Diagram::FindNode(diagram, node_id);
              node.RemoveTag(tag->name);
            }
          });
  }

  Expects(false);
}
}  // namespace vh::ponc::coreui