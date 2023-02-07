#ifndef VH_ESC_APP_H_
#define VH_ESC_APP_H_

#include <application.h>
#include <imgui_node_editor.h>

#include <memory>

#include "esc_auto_incrementable.h"
#include "esc_nodes_and_links.h"
#include "esc_textures_handle.h"

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imfilebrowser.h>
#include <imgui_internal.h>

#include <algorithm>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "esc_editor_context_handle.h"

// NOLINTNEXTLINE(*-multiple-inheritance)
class App : public Application, public std::enable_shared_from_this<App> {
 public:
  App(const char* name, int argc, char** argv);

  App(const App&) = delete;
  App(App&&) noexcept = delete;

  auto operator=(const App&) noexcept -> App& = delete;
  auto operator=(App&&) noexcept -> App& = delete;

  virtual ~App() = default;

 private:
  auto GetNextLinkId();

  void OnStart() override;
  void OnStop() override;
  void OnFrame(float deltaTime) override;

  void ShowFlow();

  //

  auto CreateEditorConfig();

  void AddInitialNodes();
  void AddInitialLinks();

  void DrawLeftPane(float paneWidth);
  void DrawFrame();

  std::optional<esc::EditorContextHandle> editor_context_{};
  std::optional<esc::TexturesHandle> textures_{};
  ImGui::FileBrowser file_browser_{};

  std::shared_ptr<esc::AutoIncrementable> auto_object_id_{};

  esc::NodesAndLinks nodes_and_links_;
};

#endif  // VH_ESC_APP_H_
