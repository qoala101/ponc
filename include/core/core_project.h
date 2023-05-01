#ifndef VH_PONC_CORE_PROJECT_H_
#define VH_PONC_CORE_PROJECT_H_

#include "core_diagram.h"
#include "core_i_family.h"
#include "core_id_generator.h"
#include "core_settings.h"
#include "core_version.h"

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
  Project(Version version, Settings settings,
          std::vector<std::unique_ptr<IFamily>> families,
          std::vector<Diagram> diagrams);

  ///
  auto GetVersion() const -> Version;
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
  Version version_{};
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
