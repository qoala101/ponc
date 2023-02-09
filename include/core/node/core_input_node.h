#ifndef VH_CORE_INPUT_NODE_H_
#define VH_CORE_INPUT_NODE_H_

#include "core_node.h"
#include "esc_id_generator.h"

class InputNode : public Node {
  public:
  explicit InputNode(esc::IdGenerator& id_generator);
};

#endif  // VH_CORE_INPUT_NODE_H_
