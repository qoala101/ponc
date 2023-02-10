#ifndef VH_JSON_DIAGRAM_SERIALIZER_H_
#define VH_JSON_DIAGRAM_SERIALIZER_H_

#include "core_diagram.h"
#include "core_node.h"
#include "crude_json.h"

namespace esc::json {
class DiagramSerializer {
 public:
  static auto ParseFromJson(
      const crude_json::value &json,
      const std::vector<std::unique_ptr<INodeFactoryParser>>
          &node_factory_parsers) -> core::Diagram;
  static auto WriteToJson(const core::Diagram &diagram) -> crude_json::value;
};
}  // namespace esc::json

#endif  // VH_JSON_DIAGRAM_SERIALIZER_H_
