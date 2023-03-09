#ifndef VH_CORE_I_FAMILY_GROUP_H_
#define VH_CORE_I_FAMILY_GROUP_H_

#include <memory>
#include <vector>

#include "core_i_family.h"
#include "core_id_generator.h"
#include "cpp_interface.h"
#include "json_i_family_parser.h"

namespace esc::core {
///
class IFamilyGroup : public cpp::Interface {
 public:
  ///
  static auto CreateDefaultFamilyGroups()
      -> std::vector<std::unique_ptr<IFamilyGroup>>;

  ///
  virtual auto CreateFamilies(IdGenerator &id_generator) const
      -> std::vector<std::unique_ptr<IFamily>> = 0;
  ///
  virtual auto CreateFamilyParser() const
      -> std::unique_ptr<json::IFamilyParser> = 0;
};
}  // namespace esc::core

#endif  // VH_CORE_I_FAMILY_GROUP_H_
