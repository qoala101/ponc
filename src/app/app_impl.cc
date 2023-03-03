#include "app_impl.h"

#include <memory>
#include <vector>

#include "app_input_family_group.h"
#include "core_i_family_group.h"
#include "coreui_project.h"

namespace esc {
///
AppImpl::AppImpl(coreui::TexturesHandle textures_handle)
    : project_{
          []() {
            auto family_groups =
                std::vector<std::unique_ptr<core::IFamilyGroup>>{};
            family_groups.emplace_back(std::make_unique<InputFamilyGroup>());
            return family_groups;
          }(),
          std::move(textures_handle)} {}

///
void AppImpl::OnFrame() {
  project_.OnFrame();
  main_window_.Draw(project_);
}
}  // namespace esc