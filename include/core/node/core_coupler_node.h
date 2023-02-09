#ifndef VH_CORE_COUPLER_NODE_H_
#define VH_CORE_COUPLER_NODE_H_

#include "core_node.h"
#include "esc_id_generator.h"

class CouplerNode : public Node {
 public:
  explicit CouplerNode(esc::IdGenerator& id_generator);

  auto GetCouplerPercentageIndex [[nodiscard]] () -> int&;
  void SetCouplerPercentageIndex(int index);

 private:
  int coupler_percentage_index_{};
};

#endif  // VH_CORE_COUPLER_NODE_H_
