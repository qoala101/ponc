/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_APP_INPUT_FAMILY_GROUP_H_
#define VH_PONC_APP_INPUT_FAMILY_GROUP_H_

#include <memory>
#include <vector>

#include "core_i_family.h"
#include "core_i_family_group.h"
#include "core_id_generator.h"
#include "json_i_family_parser.h"

namespace vh::ponc {
///
class InputFamilyGroup : public core::IFamilyGroup {
 public:
  ///
  auto CreateFamilies(core::IdGenerator &id_generator) const
      -> std::vector<std::unique_ptr<core::IFamily>> override;
  ///
  auto CreateFamilyParser() const
      -> std::unique_ptr<json::IFamilyParser> override;
};
}  // namespace vh::ponc

#endif  // VH_PONC_APP_INPUT_FAMILY_GROUP_H_
