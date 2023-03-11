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
  Project(const Settings &settings,
          std::vector<std::unique_ptr<IFamily>> families,
          Diagram diagram = Diagram{});

  ///
  static auto GetDefaultFamily(
      const std::vector<std::unique_ptr<IFamily>> &families,
      FamilyType family_type) -> const IFamily &;

  ///
  auto GetSettings() const -> const Settings &;
  ///
  auto GetSettings() -> Settings &;
  ///
  auto GetFamilies() const -> const std::vector<std::unique_ptr<IFamily>> &;
  ///
  auto GetDiagram() const -> const Diagram &;
  ///
  auto GetDiagram() -> Diagram &;
  ///
  auto GetIdGenerator() const -> const IdGenerator &;
  ///
  auto GetIdGenerator() -> IdGenerator &;

 private:
  ///
  auto FindMaxId() const;

  ///
  Settings settings_{};
  ///
  std::vector<std::unique_ptr<IFamily>> families_{};
  ///
  Diagram diagram_{};
  ///
  IdGenerator id_generator_{};
};
}  // namespace esc::core

#endif  // VH_CORE_PROJECT_H_
