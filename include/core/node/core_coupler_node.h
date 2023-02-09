#ifndef VH_CORE_COUPLER_NODE_H_
#define VH_CORE_COUPLER_NODE_H_

#include "core_node.h"
#include "esc_id_generator.h"

// NOLINTNEXTLINE(*-multiple-inheritance)
class CouplerNode : public Node,
                    public std::enable_shared_from_this<CouplerNode> {
 public:
  explicit CouplerNode(esc::IdGenerator& id_generator);

  auto GetDrawer [[nodiscard]] ()
  -> std::unique_ptr<vh::esc::ui::INodeDrawer> override;

  auto GetCouplerPercentageIndex [[nodiscard]] () -> int&;
  void SetCouplerPercentageIndex(int index);

 private:
  int coupler_percentage_index_{};
};

#endif  // VH_CORE_COUPLER_NODE_H_
