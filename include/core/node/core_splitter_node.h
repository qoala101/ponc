#ifndef VH_CORE_SPLITTER_NODE_H_
#define VH_CORE_SPLITTER_NODE_H_

#include "core_node.h"
#include "esc_id_generator.h"

struct SplitterNode : public Node {
  explicit SplitterNode(esc::IdGenerator& id_generator, int n);
};

#endif  // VH_CORE_SPLITTER_NODE_H_
