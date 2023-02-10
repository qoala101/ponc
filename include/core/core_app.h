#ifndef VH_CORE_APP_H_
#define VH_CORE_APP_H_

#include <optional>

#include "core_diagram.h"

namespace esc::core {
class App {
 public:
  auto GetDiagram() const -> const Diagram *;
  auto GetDiagram() -> Diagram *;

  void SetDiagram(Diagram diagram);

 private:
  std::optional<Diagram> diagram_{};
};
}  // namespace esc::core

#endif  // VH_CORE_APP_H_
