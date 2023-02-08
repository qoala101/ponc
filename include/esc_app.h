#ifndef VH_ESC_APP_H_
#define VH_ESC_APP_H_

#include <application.h>
#include <imgui_node_editor.h>

#include <memory>

#include "esc_left_pane.h"
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
  auto GetNextLinkId [[nodiscard]] () -> ne::LinkId;
  auto GetTextures [[nodiscard]] () -> esc::TexturesHandle&;
  auto GetNodesAndLinks [[nodiscard]] () -> esc::NodesAndLinks&;

  void ShowFlow();

  auto GetTextureDims [[nodiscard]] (ImTextureID texture_id) -> ImVec2;
  
  void OnStart() override;
  void OnStop() override;

 private:
  void OnFrame(float deltaTime) override;

  //

  void DrawContextMenu();
  void DrawFrame();
  void DrawBlueprintNode(Node &node);
  void DrawLinks();
  void DrawLinkConnection();

  auto CalculateAlphaForPin(const Pin &pin);

  std::optional<esc::EditorContextHandle> editor_context_{};
  std::optional<esc::TexturesHandle> textures_{};
  std::optional<esc::LeftPane> left_pane_;
  std::optional<esc::NodesAndLinks> nodes_and_links_;

  int next_object_id_{};

  ne::NodeId context_node_id{};
  ne::LinkId context_link_id{};
  ne::PinId context_pin_id{};
  bool create_new_node{};
  Pin* existing_node_new_link_pin{};
  Pin* new_link_pin{};

  float left_pane_width{};
  float right_pane_width{};
};

#endif  // VH_ESC_APP_H_
