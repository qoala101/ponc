#include "coreui_family.h"

#include "coreui_i_family_traits.h"

namespace esc::coreui {
///
Family::Family(cpp::SafePtr<const core::IFamily> family,
               cpp::SafePtr<core::IdGenerator> id_generator,
               Callbacks callbacks)
    : family_{std::move(family)},
      id_generator_{std::move(id_generator)},
      callbacks_{std::move(callbacks)} {}

///
auto Family::GetLabel() const -> std::string {
  return family_->CreateUiTraits()->GetLabel();
}

///
void Family::CreateNodeAt(const ImVec2 &pos) const {
  auto node = family_->CreateNode(*id_generator_);
  node->SetPos(pos);

  callbacks_.node_created(std::move(node));
}
}  // namespace esc::coreui