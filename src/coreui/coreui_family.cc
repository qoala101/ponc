#include "coreui_family.h"

#include "coreui_i_family_traits.h"

namespace esc::coreui {
///
Family::Family(cpp::SafePtr<const core::IFamily> family,
               cpp::SafePtr<core::IdGenerator> id_generator)
    : family_{std::move(family)}, id_generator_{std::move(id_generator)} {}

///
auto Family::GetFamily() const -> const core::IFamily& { return *family_; }

///
auto Family::GetLabel() const -> std::string {
  return family_->CreateUiTraits()->GetLabel();
}

///
auto Family::CreateNode() const -> std::unique_ptr<core::INode> {
  return family_->CreateNode(*id_generator_);
}
}  // namespace esc::coreui