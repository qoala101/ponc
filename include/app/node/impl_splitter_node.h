#ifndef VH_IMPL_SPLITTER_NODE_H_
#define VH_IMPL_SPLITTER_NODE_H_

#include <memory>

#include "core_i_family.h"
#include "json_i_family_parser.h"

namespace esc::impl {
// ---
struct SplitterNode {
  // ---
  static auto CreateFamily [[nodiscard]] (int num_outputs)
  -> std::shared_ptr<core::IFamily>;
  // ---
  static auto CreateFamilyParser [[nodiscard]] ()
  -> std::unique_ptr<json::IFamilyParser>;
};
}  // namespace esc::impl

#endif  // VH_IMPL_SPLITTER_NODE_H_
