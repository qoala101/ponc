#ifndef VH_IMPL_SPLITTER_NODE_H_
#define VH_IMPL_SPLITTER_NODE_H_

#include <memory>

#include "core_i_node_factory.h"
#include "json_node_factory_serializer.h"

namespace esc::impl {
// ---
struct SplitterNode {
  // ---
  static auto CreateNodeFactory [[nodiscard]] (int num_outputs)
  -> std::shared_ptr<core::INodeFactory>;
  // ---
  static auto CreateNodeFactoryParser [[nodiscard]] ()
  -> std::unique_ptr<json::INodeFactoryParser>;
};
}  // namespace esc::impl

#endif  // VH_IMPL_SPLITTER_NODE_H_
