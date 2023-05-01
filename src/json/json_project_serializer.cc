#include "json_project_serializer.h"

#include <cstdint>
#include <memory>

#include "core_diagram.h"
#include "core_i_family.h"
#include "core_project.h"
#include "core_version.h"
#include "cpp_assert.h"
#include "crude_json.h"
#include "json_container_serializer.h"
#include "json_diagram_serializer.h"
#include "json_i_family_writer.h"
#include "json_setings_serializer.h"

namespace vh::ponc::json {
namespace {
///
auto ParseVersion(const crude_json::value& json) {
  if (!json.contains("version")) {
    return core::Version::kPreCalculator;
  }

  return static_cast<core::Version>(json["version"].get<crude_json::number>());
}

///
auto ParseFamily(
    const crude_json::value& json,
    const std::vector<std::unique_ptr<IFamilyParser>>& family_parsers) {
  for (const auto& parser : family_parsers) {
    auto parsed_family = parser->TryToParseFromJson(json);

    if (parsed_family.has_value()) {
      return std::move(*parsed_family);
    }
  }

  Expects(false);
}
}  // namespace

///
auto ProjectSerializer::ParseFromJson(
    const crude_json::value& json,
    const std::vector<std::unique_ptr<IFamilyParser>>& family_parsers)
    -> core::Project {
  const auto version = ParseVersion(json);
  const auto settings = SettingsSerializer::ParseFromJson(json["settings"]);

  auto families =
      ContainerSerializer::ParseFromJson<std::unique_ptr<core::IFamily>>(
          json, "families", [&family_parsers](const auto& json) {
            return ParseFamily(json, family_parsers);
          });

  auto diagrams = ContainerSerializer::ParseFromJson<core::Diagram>(
      json, "diagrams", [&families](const auto& json) {
        return DiagramSerializer::ParseFromJson(json, families);
      });

  return core::Project{version, settings, std::move(families),
                       std::move(diagrams)};
}

///
auto ProjectSerializer::WriteToJson(const core::Project& project)
    -> crude_json::value {
  auto json = crude_json::value{};
  json["version"] = static_cast<crude_json::number>(project.GetVersion());
  json["settings"] = SettingsSerializer::WriteToJson(project.GetSettings());

  ContainerSerializer::WriteToJson(
      json, project.GetFamilies(), "families", [](const auto& family) {
        return family->CreateWriter()->WriteToJson(*family);
      });

  ContainerSerializer::WriteToJson(json, project.GetDiagrams(), "diagrams",
                                   &DiagramSerializer::WriteToJson);

  return json;
}
}  // namespace vh::ponc::json