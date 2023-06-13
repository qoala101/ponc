/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_COREUI_TAGS_H_
#define VH_PONC_COREUI_TAGS_H_

#include <imgui_node_editor.h>

#include "core_tag.h"
#include "coreui_event.h"
#include "cpp_safe_ptr.h"

namespace ne = ax::NodeEditor;

namespace vh::ponc::coreui {
///
class Project;

///
class Tags {
 public:
  ///
  explicit Tags(cpp::SafePtr<Project> parent_project);

  ///
  auto GetTags() const -> const core::Tags &;
  ///
  auto AddTag(std::shared_ptr<core::Tag> tag) const -> Event &;
  ///
  auto DeleteTag(std::string tag_name) const -> Event &;
  ///
  auto AddTagToNode(std::shared_ptr<core::Tag> tag, ne::NodeId node_id) const
      -> Event &;
  ///
  auto RemoveTagFromNode(std::string tag_name, ne::NodeId node_id) const
      -> Event &;

 private:
  ///
  cpp::SafePtr<Project> parent_project_;
};
}  // namespace vh::ponc::coreui

#endif  // VH_PONC_COREUI_TAGS_H_
