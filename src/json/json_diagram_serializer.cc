#include "json_diagram_serializer.h"

#include <memory>

#include "core_diagram.h"
#include "core_link.h"
#include "crude_json.h"
#include "json_i_family_writer.h"
#include "json_link_serializer.h"

namespace esc::json {
namespace {
auto ParseFamilies [[nodiscard]] (
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

auto ParseLinks [[nodiscard]] (const crude_json::value& json) {
  const auto links_size = json["links_size"].get<crude_json::number>();
  const auto& links_json = json["links"];

  auto parsed_links = std::vector<core::Link>{};

  for (auto i = 0; i < links_size; ++i) {
    parsed_links.emplace_back(LinkSerializer::ParseFromJson(links_json[i]));
  }

  return parsed_links;
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

void WriteLinks(const std::vector<core::Link>& links, crude_json::value& json) {
  json["links_size"] = static_cast<crude_json::number>(links.size());
  auto& links_json = json["links"];

  for (auto i = 0; i < static_cast<int>(links.size()); ++i) {
    links_json[i] = LinkSerializer::WriteToJson(links[i]);
  }
}
}  // namespace

auto DiagramSerializer::ParseFromJson(
    const crude_json::value& json,
    const std::vector<std::unique_ptr<IFamilyParser>>& family_parsers)
    -> core::Diagram {
  auto parsed_families = ParseFamilies(json, family_parsers);
  auto parsed_links = ParseLinks(json);
  return core::Diagram{std::move(parsed_families), std::move(parsed_links)};
}

auto DiagramSerializer::WriteToJson(const core::Diagram& diagram)
    -> crude_json::value {
  auto json = crude_json::value{};
  WriteFamilies(diagram.GetFamilies(), json);
  WriteLinks(diagram.GetLinks(), json);
  // WritePositions
  return json;
}
}  // namespace esc::json