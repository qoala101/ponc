#ifndef VH_IMPL_HUB_NODE_H_
#define VH_IMPL_HUB_NODE_H_

#include <memory>

#include "core_i_family.h"
#include "json_i_family_parser.h"

namespace esc {
// ---
struct HubNode {
  // ---
  static auto CreateFamily [[nodiscard]] () -> std::shared_ptr<core::IFamily>;
  // ---
  static auto CreateFamilyParser [[nodiscard]] ()
  -> std::unique_ptr<json::IFamilyParser>;
};
}  // namespace esc

#endif  // VH_IMPL_HUB_NODE_H_
