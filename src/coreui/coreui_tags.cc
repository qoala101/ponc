/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "coreui_tags.h"

#include "coreui_project.h"

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
auto Tags::AddTagToNode(std::shared_ptr<core::Tag> tag,
                        ne::NodeId node_id) const -> Event& {
  return parent_project_->GetEventLoop().PostEvent(
      [parent_project = parent_project_, tag = std::move(tag), node_id]() {
        const auto& diagram = parent_project->GetDiagram().GetDiagram();
        auto& node = core::Diagram::FindNode(diagram, node_id);
        node.AddTag(tag);
      });
}

///
auto Tags::RemoveTagFromNode(std::string tag_name, ne::NodeId node_id) const
    -> Event& {
  return parent_project_->GetEventLoop().PostEvent(
      [parent_project = parent_project_, tag_name = std::move(tag_name),
       node_id]() {
        const auto& diagram = parent_project->GetDiagram().GetDiagram();
        auto& node = core::Diagram::FindNode(diagram, node_id);
        node.RemoveTag(tag_name);
      });
}
}  // namespace vh::ponc::coreui