#include "app_impl.h"

#include <memory>
#include <vector>

#include "app_input_generation.h"
#include "core_i_generation.h"
#include "coreui_project.h"

namespace esc {
///
AppImpl::AppImpl(coreui::TexturesHandle textures_handle)
    : project_{[]() {
                 auto generations =
                     std::vector<std::unique_ptr<core::IGeneration>>{};
                 generations.emplace_back(std::make_unique<InputGeneration>());
                 return generations;
               }(),
               std::move(textures_handle)} {}

///
void AppImpl::OnFrame() {
  project_.OnFrame();
  main_window_.Draw(project_);
}
}  // namespace esc