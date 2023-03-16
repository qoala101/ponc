#include "app_app.h"

#include "coreui_project.h"
#include "coreui_texture.h"
#include "coreui_textures_handle.h"
#include "cpp_assert.h"

namespace esc {
///
App::App(const char* name, int argc, char** argv)
    : Application{name, argc, argv} {}

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
      coreui::TexturesHandle{
          {.load_texture =
               [safe_this = safe_owner_.MakeSafe(this)](auto file_path) {
                 return safe_this->LoadTexture(file_path);
               },
           .destroy_texture =
               [safe_this = safe_owner_.MakeSafe(this)](auto texture_id) {
                 return safe_this->DestroyTexture(texture_id);
               }}},
      coreui::Project::Callbacks{
          .name_changed = [safe_this =
                               safe_owner_.MakeSafe(this)](auto file_name) {
            const auto title = file_name + " - PON Calculator";
            safe_this->SetTitle(title.c_str());
          }});
}

///
void App::OnStop() {
  Expects(app_.has_value());

  app_.reset();
}

///
void App::OnFrame(float /*unused*/) {
  Expects(app_.has_value());

  app_->OnFrame();
}
}  // namespace esc