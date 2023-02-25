#include "json_project_serializer.h"

#include <cstdint>

#include "core_project.h"
#include "crude_json.h"
#include "json_diagram_serializer.h"
#include "json_i_family_writer.h"
#include "json_setings_serializer.h"

namespace esc::json {
namespace {
auto ParseFamilies(
    const crude_json::value& json,
    const std::vector<std::unique_ptr<IFamilyParser>>& family_parsers) {
  const auto families_size = json["families_size"].get<crude_json::number>();
  const auto& families_json = json["families"];

  auto parsed_families = std::vector<std::shared_ptr<core::IFamily>>{};

  for (auto i = 0; i < families_size; ++i) {
    const auto& family_json = families_json[i];

    for (const auto& parser : family_parsers) {
      auto parsed_family = parser->TryToParseFromJson(family_json);

      if (!parsed_family.has_value()) {
        continue;
      }

      parsed_families.emplace_back(std::move(*parsed_family));
    }
  }

  return parsed_families;
}

void WriteFamilies(const std::vector<std::shared_ptr<core::IFamily>>& families,
                   crude_json::value& json) {
  json["families_size"] = static_cast<crude_json::number>(families.size());
  auto& families_json = json["families"];

  for (auto i = 0; i < static_cast<int>(families.size()); ++i) {
    const auto& family = families[i];
    families_json[i] = family->CreateWriter()->WriteToJson(*family);
  }
}
}  // namespace

auto ProjectSerializer::ParseFromJson(
    const crude_json::value& json,
    const std::vector<std::unique_ptr<IFamilyParser>>& family_parsers)
    -> core::Project {
  auto families = ParseFamilies(json, family_parsers);
  auto diagram = DiagramSerializer::ParseFromJson(json["diagram"], families);
  const auto settings = SettingsSerializer::ParseFromJson(json["settings"]);
  return core::Project{std::move(families), std::move(diagram), settings};
}

auto ProjectSerializer::WriteToJson(const core::Project& project)
    -> crude_json::value {
  auto json = crude_json::value{};
  WriteFamilies(project.GetFamilies(), json);
  json["diagram"] = DiagramSerializer::WriteToJson(project.GetDiagram());
  json["settings"] = SettingsSerializer::WriteToJson(project.GetSettings());
  return json;
}
}  // namespace esc::json