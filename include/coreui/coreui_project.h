#ifndef VH_PONC_COREUI_PROJECT_H_
#define VH_PONC_COREUI_PROJECT_H_

#include <imgui_node_editor.h>

#include <filesystem>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "core_i_family_group.h"
#include "core_project.h"
#include "coreui_diagram.h"
#include "coreui_event_loop.h"
#include "coreui_textures_handle.h"
#include "cpp_callbacks.h"
#include "cpp_safe_ptr.h"

namespace vh::ponc::coreui {
///
class Project {
 public:
  ///
  struct Callbacks {
    ///
    cpp::Signal<std::string> name_changed{};
  };

  ///
  Project(std::vector<std::unique_ptr<core::IFamilyGroup>> family_groups,
          TexturesHandle textures_handle, Callbacks callbacks);

  ///
  void OnFrame();
  ///
  auto GetProject() const -> const core::Project &;
  ///
  auto GetProject() -> core::Project &;
  ///
  auto GetDiagram() const -> const Diagram &;
  ///
  auto GetDiagram() -> Diagram &;
  ///
  auto AddDiagram(core::Diagram diagram) -> Event &;
  ///
  auto CloneDiagram(const core::Diagram &diagram) -> Event &;
  ///
  auto DeleteDiagram(int index) -> Event &;
  ///
  auto SetDiagram(int index) -> Event &;
  ///
  auto GetTexturesHandle() -> TexturesHandle &;
  ///
  auto GetEventLoop() -> EventLoop &;
  ///
  auto Reset() -> Event &;
  ///
  auto OpenFromFile(std::filesystem::path file_path) -> Event &;
  ///
  auto CanSave() const -> bool;
  ///
  auto Save() -> Event &;
  ///
  auto SaveToFile(std::filesystem::path file_path) -> Event &;

 private:
  ///
  auto CreateProject() const;
  ///
  auto CreateFamilyParsers() const;
  ///
  auto GetName() const -> std::string;
  ///
  void RewireIds(const std::vector<core::IdPtr> &ids);
  ///
  void SetDiagramImpl(int index);
  ///
  void SetFilePath(std::filesystem::path file_path);

  ///
  std::vector<std::unique_ptr<core::IFamilyGroup>> family_groups_{};
  ///
  TexturesHandle textures_handle_;
  ///
  Callbacks callbacks_{};
  ///
  cpp::SafeOwner safe_owner_{};
  ///
  EventLoop event_loop_{};
  ///
  std::filesystem::path file_path_{};
  ///
  core::Project project_;
  ///
  std::unique_ptr<Diagram> diagram_{};
};
}  // namespace vh::ponc::coreui

#endif  // VH_PONC_COREUI_PROJECT_H_
