#ifndef VH_IMPL_INPUT_NODE_H_
#define VH_IMPL_INPUT_NODE_H_

#include <memory>

#include "core_family_id.h"
#include "core_i_family.h"
#include "json_i_family_parser.h"

namespace esc {
// ---
struct InputNode {
  // ---
  static auto CreateFamily [[nodiscard]] (core::FamilyId id)
  -> std::shared_ptr<core::IFamily>;
  // ---
  static auto CreateFamilyParser [[nodiscard]] ()
  -> std::unique_ptr<json::IFamilyParser>;
};
}  // namespace esc

#endif  // VH_IMPL_INPUT_NODE_H_
