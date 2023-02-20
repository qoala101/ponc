/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#ifndef VH_JSON_DIAGRAM_SERIALIZER_H_
#define VH_JSON_DIAGRAM_SERIALIZER_H_

#include <memory>

#include "core_diagram.h"
#include "core_i_family.h"
#include "crude_json.h"

namespace esc::json {
// ---
struct DiagramSerializer {
  // ---
  static auto ParseFromJson [[nodiscard]] (
      const crude_json::value &json,
      const std::vector<std::shared_ptr<core::IFamily>> &families)
  -> core::Diagram;
  // ---
  static auto WriteToJson [[nodiscard]] (const core::Diagram &diagram)
  -> crude_json::value;
};
}  // namespace esc::json

#endif  // VH_JSON_DIAGRAM_SERIALIZER_H_
