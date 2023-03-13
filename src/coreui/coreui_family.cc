#include "coreui_family.h"

#include "coreui_i_family_traits.h"
#include "coreui_project.h"

namespace esc::coreui {
///
Family::Family(cpp::SafePtr<Project> parent_project,
               cpp::SafePtr<const core::IFamily> family)
    : parent_project_{std::move(parent_project)}, family_{std::move(family)} {}

///
auto Family::GetFamily() const -> const core::IFamily& { return *family_; }

///
auto Family::GetLabel() const -> std::string {
  return family_->CreateUiTraits()->GetLabel();
}

///
auto Family::CreateNode() const -> std::unique_ptr<core::INode> {
  return family_->CreateNode(parent_project_->GetProject().GetIdGenerator());
}
}  // namespace esc::coreui