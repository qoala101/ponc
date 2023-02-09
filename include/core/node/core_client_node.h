#ifndef VH_CORE_CLIENT_NODE_H_
#define VH_CORE_CLIENT_NODE_H_

#include "core_node.h"
#include "esc_id_generator.h"

struct ClientNode : public Node {
  explicit ClientNode(esc::IdGenerator& id_generator);
};

#endif  // VH_CORE_CLIENT_NODE_H_
