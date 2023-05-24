/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "app_impl.h"

#include <memory>
#include <utility>
#include <vector>

#include "app_attenuator_family_group.h"
#include "app_client_family_group.h"
#include "app_coupler_family_group.h"
#include "app_input_family_group.h"
#include "app_splitter_family_group.h"
#include "core_i_family_group.h"
#include "coreui_project.h"

namespace vh::ponc {
///
AppImpl::AppImpl(coreui::Project::Callbacks project_callbacks)
    : project_{
          []() {
            auto family_groups =
                std::vector<std::unique_ptr<core::IFamilyGroup>>{};
            family_groups.reserve(5);

            family_groups.emplace_back(std::make_unique<InputFamilyGroup>());
            family_groups.emplace_back(std::make_unique<ClientFamilyGroup>());
            family_groups.emplace_back(std::make_unique<SplitterFamilyGroup>());
            family_groups.emplace_back(std::make_unique<CouplerFamilyGroup>());
            family_groups.emplace_back(
                std::make_unique<AttenuatorFamilyGroup>());

            return family_groups;
          }(),
          std::move(project_callbacks)} {}

///
void AppImpl::OnFrame() {
  project_.OnFrame();
  main_window_.Draw(project_);
}
}  // namespace vh::ponc