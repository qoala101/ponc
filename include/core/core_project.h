#ifndef VH_CORE_PROJECT_H_
#define VH_CORE_PROJECT_H_

#include "core_diagram.h"
#include "core_i_family.h"
#include "core_id_generator.h"
#include "core_settings.h"
#include "cpp_safe_ptr.h"

namespace esc::core {
///
class Project {
 public:
  ///
  explicit Project(std::vector<std::unique_ptr<IFamily>> families = {},
                   Diagram diagram = Diagram{}, const Settings &settings = {});

  ///
  auto GetIdGenerator() const -> const IdGenerator &;
  ///
  auto GetIdGenerator() -> IdGenerator &;
  ///
  auto GetFamilies() const -> const std::vector<std::unique_ptr<IFamily>> &;
  ///
  auto GetDiagram() const -> const Diagram &;
  ///
  auto GetDiagram() -> Diagram &;
  ///
  auto GetSettings() const -> const Settings &;
  ///
  auto GetSettings() -> Settings &;

 private:
  ///
  IdGenerator id_generator_{};
  ///
  std::vector<std::unique_ptr<IFamily>> families_{};
  ///
  Diagram diagram_{};
  ///
  Settings settings_{};
};
}  // namespace esc::core

#endif  // VH_CORE_PROJECT_H_
