/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#include "app_app.h"

#include "app_textures.h"
#include "cpp_assert.h"
#include "draw_texture.h"

namespace esc {
// ---
App::App(const char* name, int argc, char** argv)
    : Application{name, argc, argv} {}

// ---
auto App::GetWindowFlags() const -> ImGuiWindowFlags {
  // NOLINTNEXTLINE(*-signed-bitwise)
  return Application::GetWindowFlags() | ImGuiWindowFlags_MenuBar;
}

// ---
auto App::LoadTexture(std::string_view file_path) {
  auto texture =
      draw::Texture{.id = Application::LoadTexture(file_path.data())};
  texture.width = GetTextureWidth(texture.id);
  texture.height = GetTextureHeight(texture.id);
  return texture;
}

// ---
void App::OnStart() {
  Expects(!textures_.has_value());
  Expects(!impl_.has_value());

  textures_ =
      Textures{.node_header = LoadTexture("data/node_header_texture.png")};
  impl_.emplace(*textures_);

  Ensures(textures_.has_value());
  Ensures(impl_.has_value());
}

// ---
void App::OnStop() {
  Expects(impl_.has_value());
  Expects(textures_.has_value());

  impl_.reset();

  DestroyTexture(textures_->node_header.id);
  textures_.reset();

  Ensures(!impl_.has_value());
  Ensures(!textures_.has_value());
}

// ---
void App::OnFrame(float /*unused*/) {
  Expects(impl_.has_value());

  impl_->OnFrame();
}
}  // namespace esc