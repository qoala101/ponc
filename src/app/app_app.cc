#include "app_app.h"

#include "coreui_texture.h"
#include "coreui_textures_handle.h"
#include "cpp_assert.h"

namespace esc {
///
App::App(const char* name, int argc, char** argv)
    : Application{name, argc, argv}, EnableSafePointer{this} {}

///
auto App::GetWindowFlags() const -> ImGuiWindowFlags {
  // NOLINTNEXTLINE(*-signed-bitwise)
  return Application::GetWindowFlags() | ImGuiWindowFlags_MenuBar;
}

///
auto App::LoadTexture(std::string_view file_path) {
  auto texture =
      coreui::Texture{.id = Application::LoadTexture(file_path.data())};
  texture.width = GetTextureWidth(texture.id);
  texture.height = GetTextureHeight(texture.id);
  return texture;
}

///
void App::OnStart() {
  Expects(!app_.has_value());

  app_.emplace(
      coreui::TexturesHandle{{.load_texture =
                                  [app = CreateSafePointer()](auto file_path) {
                                    return app->LoadTexture(file_path);
                                  },
                              .destroy_texture =
                                  [app = CreateSafePointer()](auto texture_id) {
                                    return app->DestroyTexture(texture_id);
                                  }}});

  Ensures(app_.has_value());
}

///
void App::OnStop() {
  Expects(app_.has_value());

  app_.reset();

  Ensures(!app_.has_value());
}

///
void App::OnFrame(float /*unused*/) {
  Expects(app_.has_value());

  app_->OnFrame();
}
}  // namespace esc