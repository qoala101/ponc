#include "core_app.h"

namespace esc::core {
auto App::GetDiagram() const -> const Diagram * {
  // NOLINTNEXTLINE(*-const-cast)
  return const_cast<App *>(this)->GetDiagram();
}

auto App::GetDiagram() -> Diagram * {
  if (!diagram_.has_value()) {
    return nullptr;
  }

  return &*diagram_;
}

void App::SetDiagram(Diagram diagram) { diagram_ = std::move(diagram); }
}  // namespace esc::core