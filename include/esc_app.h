#ifndef VH_ESC_APP_H_
#define VH_ESC_APP_H_

#include <application.h>
#include <imgui_node_editor.h>

#include <memory>
#include <optional>

#include "core_app.h"
#include "core_diagram.h"
#include "draw_link_connection_process.h"
#include "draw_main_window.h"
#include "draw_popups.h"
#include "esc_state.h"
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

  void DrawFrame();
  void DrawNode(core::INode& node);
  void DrawNodeEditor();
  void DrawNodes();
  void DrawLinks();
  void DrawDeleteItemsProcess();

  auto CanCreateLink(ne::PinId left, ne::PinId right) -> bool;
  auto CalculateAlphaForPin(ne::PinId pin_id);

  auto IsPinLinked(ne::PinId id) const -> bool;
  auto IsFlowPin(ne::PinId id, const core::INode &node) const -> bool;
  void AddLinkFromPinToNode(ne::LinkId link_id, ne::PinId pin_id,
                            core::INode& node);

  std::optional<std::shared_ptr<State>> state_{};
  std::optional<esc::EditorContextHandle> editor_context_{};
  std::optional<esc::TexturesHandle> textures_{};
  std::optional<draw::MainWindow> main_window_;
  std::optional<draw::Popups> popups_{};
  std::optional<draw::LinkConnectionProcess> link_connection_process_{};
};
}  // namespace esc

#endif  // VH_ESC_APP_H_
