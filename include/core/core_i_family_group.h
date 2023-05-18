/**
 * PON Calculator @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_CORE_I_FAMILY_GROUP_H_
#define VH_PONC_CORE_I_FAMILY_GROUP_H_

#include <memory>
#include <vector>

#include "core_i_family.h"
#include "core_id_generator.h"
#include "cpp_non_copyable.h"
#include "json_i_family_parser.h"

namespace vh::ponc::core {
///
class IFamilyGroup : public cpp::NonCopyable {
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
}  // namespace vh::ponc::core

#endif  // VH_PONC_CORE_I_FAMILY_GROUP_H_
