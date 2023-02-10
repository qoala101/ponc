#ifndef VH_ESC_APP_H_
#define VH_ESC_APP_H_

#include <application.h>
#include <imgui_node_editor.h>

#include <memory>

#include "esc_id_generator.h"
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

#include "draw_menu_bar.h"
#include "esc_editor_context_handle.h"

namespace esc {
// NOLINTNEXTLINE(*-multiple-inheritance)
class App : public Application, public std::enable_shared_from_this<App> {
 public:
  App(const char* name, int argc, char** argv);

  App(const App&) = delete;
  App(App&&) noexcept = delete;

  auto operator=(const App&) noexcept -> App& = delete;
  auto operator=(App&&) noexcept -> App& = delete;

  virtual ~App() = default;

  auto GetTextures() -> esc::TexturesHandle&;
  auto GetNodesAndLinks() -> esc::NodesAndLinks&;

  void ShowFlow();

  auto GetTextureDims(ImTextureID texture_id) -> ImVec2;

  void OnStart() override;
  void OnStop() override;
  auto GetWindowFlags() const -> ImGuiWindowFlags override;

  auto GetIdGenerator() -> esc::IdGenerator& { return *id_generator_; }

 private:
  void OnFrame(float deltaTime) override;

  //

  void DrawContextMenuProcess();
  void DrawFrame();
  void DrawNode(INode& node);
  void DrawNodeEditor();
  void DrawNodes();
  void DrawLinks();
  void DrawLinkConnectionProcess();
  void DrawDeleteItemsProcess();

  auto CalculateAlphaForPin(const Pin& pin);

  std::shared_ptr<esc::IdGenerator> id_generator_{};

  std::optional<esc::EditorContextHandle> editor_context_{};
  std::optional<esc::TexturesHandle> textures_{};
  std::optional<esc::LeftPane> left_pane_;
  std::optional<draw::MenuBar> menu_bar_;
  std::optional<esc::NodesAndLinks> nodes_and_links_;

  struct PopupState {
    ne::NodeId context_node_id{};
    ne::LinkId context_link_id{};
    ne::PinId context_pin_id{};
  } popup_state_{};

  struct DrawingState {
    Pin* not_yet_connected_pin_of_new_link{};
    Pin* connect_new_node_to_existing_pin{};
  } drawing_state_{};

  float left_pane_width{};
  float right_pane_width{};
};
}  // namespace esc

#endif  // VH_ESC_APP_H_
