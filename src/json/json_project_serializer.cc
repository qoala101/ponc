#include "json_project_serializer.h"

#include <cstdint>
#include <memory>

#include "core_i_family.h"
#include "core_project.h"
#include "cpp_assert.h"
#include "crude_json.h"
#include "json_container_serializer.h"
#include "json_diagram_serializer.h"
#include "json_i_family_writer.h"
#include "json_setings_serializer.h"

namespace esc::json {
namespace {
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
  auto families =
      ContainerSerializer::ParseFromJson<std::unique_ptr<core::IFamily>>(
          json, "families", [&family_parsers](const auto& json) {
            return ParseFamily(json, family_parsers);
          });

  auto diagram = DiagramSerializer::ParseFromJson(json["diagram"], families);
  const auto settings = SettingsSerializer::ParseFromJson(json["settings"]);
  return core::Project{std::move(families), std::move(diagram), settings};
}

///
auto ProjectSerializer::WriteToJson(const core::Project& project)
    -> crude_json::value {
  auto json = crude_json::value{};

  ContainerSerializer::WriteToJson(
      json, project.GetFamilies(), "families", [](const auto& family) {
        return family->CreateWriter()->WriteToJson(*family);
      });

  json["diagram"] = DiagramSerializer::WriteToJson(project.GetDiagram());
  json["settings"] = SettingsSerializer::WriteToJson(project.GetSettings());
  return json;
}
}  // namespace esc::json