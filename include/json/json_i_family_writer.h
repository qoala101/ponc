/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#ifndef VH_JSON_I_FAMILY_WRITER_H_
#define VH_JSON_I_FAMILY_WRITER_H_

#include "core_i_family.h"
#include "cpp_interface.h"
#include "crude_json.h"

namespace esc::json {
class IFamilyWriter : public cpp::Interface {
 public:
  auto WriteToJson(const core::IFamily &family) const -> crude_json::value;

 private:
  virtual auto GetTypeName() const -> std::string = 0;
  virtual auto WriteToJson() const -> crude_json::value = 0;
};
}  // namespace esc::json

#endif  // VH_JSON_I_FAMILY_WRITER_H_
