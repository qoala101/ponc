#include "coreui_project.h"

#include <functional>
#include <iterator>
#include <utility>
#include <variant>
#include <vector>

#include "core_diagram.h"
#include "core_id_generator.h"
#include "core_project.h"
#include "core_settings.h"
#include "coreui_diagram.h"
#include "coreui_event_loop.h"
#include "coreui_texture.h"

namespace esc::coreui {
///
Project::Project(cpp::SafePtr<core::Project> project,
                 std::vector<std::unique_ptr<core::IGeneration>> generations,
                 TexturesHandle textures_handle)
    : project_{std::move(project)},
      generations_{std::move(generations)},
      textures_handle_{std::move(textures_handle)},
      diagram_{safe_owner_.MakeSafe(&project_->GetDiagram()),
               {.is_color_flow =
                    [safe_this = safe_owner_.MakeSafe(this)]() {
                      return safe_this->project_->GetSettings().color_flow;
                    },
                .get_flow_color =
                    [safe_this = safe_owner_.MakeSafe(this)](auto flow) {
                      return core::Settings::GetFlowColor(
                          safe_this->project_->GetSettings(), flow);
                    },
                .post_event =
                    [safe_this = safe_owner_.MakeSafe(this)](auto event) {
                      safe_this->event_loop_.PostEvent(std::move(event));
                    },
                .get_texture = [safe_this = safe_owner_.MakeSafe(this)](
                                   auto file_path) -> const Texture& {
                  return safe_this->textures_handle_.GetTexture(file_path);
                }}} {
  ResetProject();
}

///
void Project::OnFrame() {
  event_loop_.ExecuteEvents();
  diagram_.OnFrame();
}

///
auto Project::GetDiagram() const -> const Diagram& {
  // NOLINTNEXTLINE(*-const-cast)
  return const_cast<Project*>(this)->GetDiagram();
}

///
auto Project::GetDiagram() -> Diagram& { return diagram_; }

///
void Project::ResetProject() const {
  auto id_generator = core::IdGenerator{};
  auto families = std::vector<std::unique_ptr<core::IFamily>>{};

  for (const auto& generation : generations_) {
    auto generation_families = generation->CreateFamilies(id_generator);

    families.insert(families.end(),
                    std::move_iterator{generation_families.begin()},
                    std::move_iterator{generation_families.end()});
  }

  *project_ = core::Project{std::move(families),
                            core::Diagram{},
                            {
                                .min_flow = -27.F,
                                .low_flow = -22.F,
                                .high_flow = -18.F,
                                .max_flow = 6.F,
                            }};
}

}  // namespace esc::coreui