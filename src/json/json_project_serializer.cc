/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#include "json_project_serializer.h"

#include <crude_json.h>

#include <algorithm>
#include <memory>
#include <optional>
#include <utility>

#include "core_diagram.h"
#include "core_i_family.h"
#include "core_project.h"
#include "cpp_assert.h"
#include "json_container_serializer.h"
#include "json_diagram_serializer.h"
#include "json_i_family_writer.h"  // IWYU pragma: keep
#include "json_setings_serializer.h"
#include "json_versifier.h"

namespace vh::ponc::json {
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
  const auto settings = SettingsSerializer::ParseFromJson(json["settings"]);

  auto families =
      ContainerSerializer::ParseFromJson<std::unique_ptr<core::IFamily>>(
          json["families"], [&family_parsers](const auto& json) {
            return ParseFamily(json, family_parsers);
          });

  auto diagrams = ContainerSerializer::ParseFromJson<core::Diagram>(
      json["diagrams"], [&families](const auto& json) {
        return DiagramSerializer::ParseFromJson(json, families);
      });

  return {settings, std::move(families), {}, std::move(diagrams)};
}

///
auto ProjectSerializer::WriteToJson(const core::Project& project)
    -> crude_json::value {
  auto json = crude_json::value{};
  json["version"] =
      static_cast<crude_json::number>(Versifier::GetCurrentVersion());
  json["settings"] = SettingsSerializer::WriteToJson(project.GetSettings());

  json["families"] = ContainerSerializer::WriteToJson(
      project.GetFamilies(), [](const auto& family) {
        return family->CreateWriter()->WriteToJson(*family);
      });

  json["diagrams"] = ContainerSerializer::WriteToJson(
      project.GetDiagrams(), &DiagramSerializer::WriteToJson);

  return json;
}
}  // namespace vh::ponc::json