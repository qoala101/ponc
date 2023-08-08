/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_COREUI_PROJECT_H_
#define VH_PONC_COREUI_PROJECT_H_

#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "app_globals.h"
#include "core_connection.h"
#include "core_diagram.h"
#include "core_i_family_group.h"
#include "core_id_ptr.h"
#include "core_project.h"
#include "coreui_calculator.h"
#include "coreui_diagram.h"
#include "coreui_event.h"
#include "coreui_event_loop.h"
#include "coreui_log.h"
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
          Callbacks callbacks);

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
  auto AddConnection() -> Event &;
  ///
  auto DeleteConnection(core::ConnectionId connection_id) -> Event &;
  ///
  auto GetCalculator() -> Calculator &;
  ///
  auto GetLog() -> Log &;
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
  ///
  Calculator calculator_;
  ///
  Log log_{};
};
}  // namespace vh::ponc::coreui

#endif  // VH_PONC_COREUI_PROJECT_H_
