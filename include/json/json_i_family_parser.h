/**
 * PON Calculator https://github.com/qoala101/ponc
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_JSON_I_FAMILY_PARSER_H_
#define VH_PONC_JSON_I_FAMILY_PARSER_H_

#include <memory>

#include "core_i_family.h"
#include "cpp_non_copyable.h"
#include "crude_json.h"

namespace vh::ponc::json {
///
class IFamilyParser : public cpp::NonCopyable {
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
}  // namespace vh::ponc::json

#endif  // VH_PONC_JSON_I_FAMILY_PARSER_H_
