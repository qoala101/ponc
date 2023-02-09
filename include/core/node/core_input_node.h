#ifndef VH_CORE_INPUT_NODE_H_
#define VH_CORE_INPUT_NODE_H_

#include "core_node.h"
#include "esc_id_generator.h"

struct InputNode : public Node {
  explicit InputNode(esc::IdGenerator& id_generator);
};

#endif  // VH_CORE_INPUT_NODE_H_
