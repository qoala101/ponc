/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#include "app_app.h"
#include "cpp_assert.h"

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
void App::OnStart() {
  Expects(!impl_.has_value());
  impl_.emplace();
  Ensures(impl_.has_value());
}

// ---
void App::OnStop() {
  Expects(impl_.has_value());
  impl_.reset();
  Ensures(!impl_.has_value());
}

// ---
void App::OnFrame(float /*unused*/) {
  Expects(impl_.has_value());
  impl_->OnFrame();
}
}  // namespace esc