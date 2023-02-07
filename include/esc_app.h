#ifndef VH_ESC_APP_H_
#define VH_ESC_APP_H_

#include <application.h>
#include <imgui_node_editor.h>

#include <memory>

#include "esc_left_panel.h"
#include "esc_nodes_and_links.h"
#include "esc_textures_handle.h"
#include "imgui.h"

#define IMGUI_DEFINE_MATH_OPERATORS
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

  auto GetNextObjectId [[nodiscard]] () -> int;
  auto GetTextures [[nodiscard]] () -> esc::TexturesHandle&;
  auto GetNodesAndLinks [[nodiscard]] () -> esc::NodesAndLinks&;

  void ShowFlow();

  auto GetTextureDims [[nodiscard]] (ImTextureID texture_id) -> ImVec2;

 private:
  auto GetNextLinkId [[nodiscard]] ();

  void OnStart() override;
  void OnStop() override;
  void OnFrame(float deltaTime) override;

  //

  auto CreateEditorConfig [[nodiscard]] ();

  void AddInitialNodes();
  void AddInitialLinks();

  void DrawFrame();

  std::optional<esc::NodesAndLinks> nodes_and_links_;
  std::optional<esc::EditorContextHandle> editor_context_{};
  std::optional<esc::TexturesHandle> textures_{};
  std::optional<esc::LeftPanel> left_panel_;

  int next_object_id_{};
};

#endif  // VH_ESC_APP_H_
