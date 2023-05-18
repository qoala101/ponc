/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_CORE_PROJECT_H_
#define VH_PONC_CORE_PROJECT_H_

#include "core_diagram.h"
#include "core_i_family.h"
#include "core_id_generator.h"
#include "core_settings.h"

namespace vh::ponc::core {
///
class Project {
 public:
  ///
  static auto IsEmpty(const Project &project) -> bool;
  ///
  static auto FindFamily(const Project &project, core::FamilyId family_id)
      -> IFamily &;
  ///
  static auto MakeUniqueDiagramName(const Project &project,
                                    std::string source_name = "Diagram",
                                    std::string_view postfix = {})
      -> std::string;

  ///
  Project(Settings settings, std::vector<std::unique_ptr<IFamily>> families,
          std::vector<Diagram> diagrams = {});

  ///
  auto GetSettings() const -> const Settings &;
  ///
  auto GetSettings() -> Settings &;
  ///
  auto GetFamilies() const -> const std::vector<std::unique_ptr<IFamily>> &;
  ///
  auto GetDiagrams() const -> const std::vector<Diagram> &;
  ///
  auto GetDiagrams() -> std::vector<Diagram> &;
  ///
  auto EmplaceDiagram(Diagram diagram) -> Diagram &;
  ///
  void DeleteDiagram(int index);
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
  std::vector<Diagram> diagrams_{};
  ///
  IdGenerator id_generator_{};
};
}  // namespace vh::ponc::core

#endif  // VH_PONC_CORE_PROJECT_H_
