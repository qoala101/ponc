#ifndef VH_JSON_DIAGRAM_SERIALIZER_H_
#define VH_JSON_DIAGRAM_SERIALIZER_H_

#include "core_diagram.h"
#include "crude_json.h"
#include "json_i_family_parser.h"

namespace esc::json {
// ---
struct DiagramSerializer {
  // ---
  static auto ParseFromJson [[nodiscard]] (
      const crude_json::value &json,
      const std::vector<std::unique_ptr<IFamilyParser>> &family_parsers)
  -> core::Diagram;
  // ---
  static auto WriteToJson [[nodiscard]] (const core::Diagram &diagram)
  -> crude_json::value;
};
}  // namespace esc::json

#endif  // VH_JSON_DIAGRAM_SERIALIZER_H_
