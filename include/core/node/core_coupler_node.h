#ifndef VH_CORE_COUPLER_NODE_H_
#define VH_CORE_COUPLER_NODE_H_

#include "core_node.h"
#include "esc_id_generator.h"

struct CouplerNode : public Node {
  explicit CouplerNode(esc::IdGenerator& id_generator);
};

#endif  // VH_CORE_COUPLER_NODE_H_
