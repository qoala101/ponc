#ifndef VH_ESC_APP_H_
#define VH_ESC_APP_H_

#include <application.h>
#include <imgui_node_editor.h>

#include <memory>

#include "core_app.h"
#include "core_diagram.h"
#include "draw_main_window.h"
#include "esc_app_state.h"
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

  auto GetTextureDims(ImTextureID texture_id) -> ImVec2;

  void OnStart() override;
  void OnStop() override;
  auto GetWindowFlags() const -> ImGuiWindowFlags override;

 private:
  void OnFrame(float deltaTime) override;

  //

  void DrawContextMenuProcess();
  void DrawFrame();
  void DrawNode(core::INode& node);
  void DrawNodeEditor();
  void DrawNodes();
  void DrawLinks();
  void DrawLinkConnectionProcess();
  void DrawDeleteItemsProcess();

  auto CanCreateLink(ne::PinId left, ne::PinId right) -> bool;
  auto CalculateAlphaForPin(ne::PinId pin_id);

  auto IsPinLinked(ne::PinId id) const -> bool;
  void AddLinkFromPinToNode(ne::LinkId link_id, ne::PinId pin_id,
                            core::INode& node);

  std::optional<std::shared_ptr<AppState>> app_state_{};
  std::optional<esc::EditorContextHandle> editor_context_{};
  std::optional<esc::TexturesHandle> textures_{};
  std::optional<draw::MainWindow> main_window_;

  struct PopupState {
    ne::NodeId context_node_id{};
    ne::LinkId context_link_id{};
    ne::PinId context_pin_id{};
  } popup_state_{};

  struct DrawingState {
    std::optional<ne::PinId> not_yet_connected_pin_of_new_link_id{};
    std::optional<ne::PinId> connect_new_node_to_existing_pin_id{};
  } drawing_state_{};
};
}  // namespace esc

#endif  // VH_ESC_APP_H_
