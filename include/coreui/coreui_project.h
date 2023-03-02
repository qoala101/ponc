#ifndef VH_COREUI_PROJECT_H_
#define VH_COREUI_PROJECT_H_

#include <imgui_node_editor.h>

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "core_project.h"
#include "coreui_diagram.h"
#include "coreui_event_loop.h"
#include "cpp_safe_pointer.h"

namespace esc::coreui {
///
class Project {
 public:
  ///
  explicit Project(cpp::SafePointer<core::Project> project);

  ///
  void OnFrame();
  ///
  auto GetDiagram() const -> const Diagram &;
  ///
  auto GetDiagram() -> Diagram &;

 private:
  ///
  cpp::SafePointer<core::Project> project_;
  ///
  EventLoop event_loop_{};
  ///
  Diagram diagram_;
};
}  // namespace esc::coreui

#endif  // VH_COREUI_PROJECT_H_
