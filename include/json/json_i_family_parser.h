#ifndef VH_JSON_I_FAMILY_PARSER_H_
#define VH_JSON_I_FAMILY_PARSER_H_

#include <memory>

#include "core_family_id.h"
#include "core_i_family.h"
#include "cpp_interface.h"
#include "crude_json.h"

namespace esc::json {
///
class IFamilyParser : public cpp::Interface {
 public:
  ///
  auto TryToParseFromJson(const crude_json::value &json) const
      -> std::optional<std::unique_ptr<core::IFamily>>;

 private:
  ///
  virtual auto GetTypeName() const -> std::string = 0;
  ///
  virtual auto ParseFromJson(core::FamilyId parsed_id,
                             const crude_json::value &json) const
      -> std::unique_ptr<core::IFamily> = 0;
};
}  // namespace esc::json

#endif  // VH_JSON_I_FAMILY_PARSER_H_
