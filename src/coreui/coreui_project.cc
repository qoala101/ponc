#include "coreui_project.h"

#include <functional>
#include <utility>
#include <variant>
#include <vector>

#include "core_diagram.h"
#include "core_settings.h"
#include "coreui_diagram.h"
#include "coreui_event_loop.h"

namespace esc::coreui {
///
Project::Project(cpp::SafePointer<core::Project> project)
    : project_{std::move(project)},
      diagram_{project_->GetDiagram().CreateSafePointer(),
               {.is_color_flow =
                    [project = project_]() {
                      return project->GetSettings().color_flow;
                    },
                .get_flow_color =
                    [project = project_](auto flow) {
                      return core::Settings::GetFlowColor(
                          project->GetSettings(), flow);
                    },
                .post_event =
                    [event_loop = event_loop_.CreateSafePointer()](auto event) {
                      event_loop->PostEvent(std::move(event));
                    }}} {}

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
}  // namespace esc::coreui