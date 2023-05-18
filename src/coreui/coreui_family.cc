/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "coreui_family.h"

#include "coreui_i_family_traits.h"
#include "coreui_project.h"

namespace vh::ponc::coreui {
///
Family::Family(cpp::SafePtr<Project> parent_project,
               cpp::SafePtr<const core::IFamily> family,
               std::vector<cpp::SafePtr<const Node>> nodes)
    : parent_project_{std::move(parent_project)},
      family_{std::move(family)},
      nodes_{std::move(nodes)} {}

///
auto Family::GetFamily() const -> const core::IFamily& { return *family_; }

///
auto Family::GetNodes() const -> const std::vector<cpp::SafePtr<const Node>>& {
  return nodes_;
}

///
auto Family::GetLabel() const -> std::string {
  return family_->CreateUiTraits()->GetLabel();
}

///
auto Family::CreateNode() const -> std::unique_ptr<core::INode> {
  return family_->CreateNode(parent_project_->GetProject().GetIdGenerator());
}
}  // namespace vh::ponc::coreui