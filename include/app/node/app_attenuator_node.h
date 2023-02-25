#ifndef VH_APP_ATTENUATOR_NODE_H_
#define VH_APP_ATTENUATOR_NODE_H_

#include <memory>

#include "core_i_family.h"
#include "json_i_family_parser.h"

namespace esc {
struct AttenuatorNode {
  static auto CreateFamily(core::FamilyId id) -> std::shared_ptr<core::IFamily>;
  static auto CreateFamilyParser() -> std::unique_ptr<json::IFamilyParser>;
};
}  // namespace esc

#endif  // VH_APP_ATTENUATOR_NODE_H_
