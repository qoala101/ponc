/**
 * PON Calculator https://github.com/qoala101/ponc
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "app_app.h"

#include "coreui_project.h"
#include "cpp_assert.h"
#include "imgui.h"
#include "style_update_styles.h"

namespace vh::ponc {
///
App::App(const char* name, int argc, char** argv)
    : Application{name, argc, argv} {}

///
auto App::GetWindowFlags() const -> ImGuiWindowFlags {
  // NOLINTNEXTLINE(*-signed-bitwise)
  return Application::GetWindowFlags() | ImGuiWindowFlags_MenuBar;
}

///
void App::OnStart() {
  style::UpdateStyle(ImGui::GetStyle());

  app_.emplace(coreui::Project::Callbacks{
      .name_changed = [safe_this = safe_owner_.MakeSafe(this)](auto file_name) {
        const auto title = std::move(file_name) + " - PON Calculator";
        safe_this->SetTitle(title.c_str());
      }});
}

///
void App::OnStop() { app_.reset(); }

///
void App::OnFrame(float /*unused*/) {
  Expects(app_.has_value());
  app_->OnFrame();
}
}  // namespace vh::ponc