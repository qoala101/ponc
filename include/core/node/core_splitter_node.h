#ifndef VH_CORE_SPLITTER_NODE_H_
#define VH_CORE_SPLITTER_NODE_H_

#include <memory>

#include "core_node.h"

namespace esc {
auto CreateSplitterNodeFactory(int num_outputs)
    -> std::shared_ptr<INodeFactory>;
}  // namespace esc

#endif  // VH_CORE_SPLITTER_NODE_H_
