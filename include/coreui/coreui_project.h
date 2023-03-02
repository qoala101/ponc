#ifndef VH_COREUI_PROJECT_H_
#define VH_COREUI_PROJECT_H_

#include <imgui_node_editor.h>

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "core_i_generation.h"
#include "core_project.h"
#include "coreui_diagram.h"
#include "coreui_event_loop.h"
#include "coreui_textures_handle.h"
#include "cpp_safe_pointer.h"

namespace esc::coreui {
///
class Project {
 public:
  ///
  Project(cpp::SafePointer<core::Project> project,
          std::vector<std::unique_ptr<core::IGeneration>> generations,
          TexturesHandle textures_handle);

  ///
  void OnFrame();
  ///
  auto GetDiagram() const -> const Diagram &;
  ///
  auto GetDiagram() -> Diagram &;

 private:
  ///
  void ResetProject() const;

  ///
  cpp::SafePointer<core::Project> project_;
  ///
  std::vector<std::unique_ptr<core::IGeneration>> generations_{};
  ///
  TexturesHandle textures_handle_;
  ///
  EventLoop event_loop_{};
  ///
  Diagram diagram_;
};
}  // namespace esc::coreui

#endif  // VH_COREUI_PROJECT_H_
