#ifndef VH_APP_ATTENUATOR_NODE_H_
#define VH_APP_ATTENUATOR_NODE_H_

#include <memory>

#include "core_i_family.h"
#include "core_i_family_group.h"
#include "json_i_family_parser.h"

namespace esc {
///
struct AttenuatorFamilyGroup : public core::IFamilyGroup {
 public:
  ///
  auto CreateFamilies(core::IdGenerator &id_generator) const
      -> std::vector<std::unique_ptr<core::IFamily>> override;
  ///
  auto CreateFamilyParser() const
      -> std::unique_ptr<json::IFamilyParser> override;
};
}  // namespace esc

#endif  // VH_APP_ATTENUATOR_NODE_H_
