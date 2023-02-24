/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#include "app_app.h"

#include "app_textures.h"
#include "cpp_assert.h"
#include "draw_texture.h"

namespace esc {
App::App(const char* name, int argc, char** argv)
    : Application{name, argc, argv} {}

auto App::GetWindowFlags() const -> ImGuiWindowFlags {
  // NOLINTNEXTLINE(*-signed-bitwise)
  return Application::GetWindowFlags() | ImGuiWindowFlags_MenuBar;
}

auto App::LoadTexture(std::string_view file_path) {
  auto texture =
      draw::Texture{.id = Application::LoadTexture(file_path.data())};
  texture.width = GetTextureWidth(texture.id);
  texture.height = GetTextureHeight(texture.id);
  return texture;
}

void App::OnStart() {
  Expects(!textures_.has_value());
  Expects(!app_.has_value());

  textures_ =
      Textures{.node_header = LoadTexture("data/node_header_texture.png")};
  app_.emplace(*textures_);

  Ensures(textures_.has_value());
  Ensures(app_.has_value());
}

void App::OnStop() {
  Expects(app_.has_value());
  Expects(textures_.has_value());

  app_.reset();

  DestroyTexture(textures_->node_header.id);
  textures_.reset();

  Ensures(!app_.has_value());
  Ensures(!textures_.has_value());
}

void App::OnFrame(float /*unused*/) {
  Expects(app_.has_value());

  app_->OnFrame();
}
}  // namespace esc