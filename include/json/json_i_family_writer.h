#ifndef VH_PONC_JSON_I_FAMILY_WRITER_H_
#define VH_PONC_JSON_I_FAMILY_WRITER_H_

#include "core_i_family.h"
#include "cpp_non_copyable.h"
#include "crude_json.h"

namespace vh::ponc::json {
///
class IFamilyWriter : public cpp::NonCopyable {
 public:
  ///
  auto WriteToJson(const core::IFamily &family) const -> crude_json::value;

 private:
  ///
  virtual auto GetTypeName() const -> std::string = 0;
  ///
  virtual auto WriteToJson() const -> crude_json::value;
};
}  // namespace vh::ponc::json

#endif  // VH_PONC_JSON_I_FAMILY_WRITER_H_
