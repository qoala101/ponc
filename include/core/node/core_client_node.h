#ifndef VH_CORE_CLIENT_NODE_H_
#define VH_CORE_CLIENT_NODE_H_

#include <memory>

#include "core_node.h"

namespace esc {
auto CreateClientNodeFactory() -> std::shared_ptr<INodeFactory>;
auto CreateClientNodeFactoryParser() -> std::unique_ptr<INodeFactoryParser>;
}  // namespace esc

#endif  // VH_CORE_CLIENT_NODE_H_
