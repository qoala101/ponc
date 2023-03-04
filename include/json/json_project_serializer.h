#ifndef VH_JSON_PROJECT_SERIALIZER_H_
#define VH_JSON_PROJECT_SERIALIZER_H_

#include "core_project.h"
#include "cpp_no_instances.h"
#include "crude_json.h"
#include "json_i_family_parser.h"

namespace esc::json {
///
struct ProjectSerializer : public cpp::NoInstances {
  ///
  static auto ParseFromJson(
      const crude_json::value &json,
      const std::vector<std::unique_ptr<IFamilyParser>> &family_parsers)
      -> core::Project;
  ///
  static auto WriteToJson(const core::Project &project) -> crude_json::value;
};
}  // namespace esc::json

#endif  // VH_JSON_PROJECT_SERIALIZER_H_
