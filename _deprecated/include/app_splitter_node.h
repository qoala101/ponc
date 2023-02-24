#ifndef VH_IMPL_SPLITTER_NODE_H_
#define VH_IMPL_SPLITTER_NODE_H_

#include <memory>

#include "core_i_family.h"
#include "json_i_family_parser.h"

namespace esc {
struct SplitterNode {
  static auto CreateFamily(int num_outputs) -> std::shared_ptr<core::IFamily>;
  static auto CreateFamilyParser() -> std::unique_ptr<json::IFamilyParser>;
};
}  // namespace esc

#endif  // VH_IMPL_SPLITTER_NODE_H_
