#ifndef VH_CORE_COUPLER_NODE_H_
#define VH_CORE_COUPLER_NODE_H_

#include "core_node.h"
#include "esc_id_generator.h"

namespace esc {
// NOLINTNEXTLINE(*-multiple-inheritance)
class CouplerNode : public Node,
                    public std::enable_shared_from_this<CouplerNode> {
 public:
  explicit CouplerNode(esc::IdGenerator& id_generator);

  auto CreateDrawer [[nodiscard]] ()
  -> std::unique_ptr<INodeDrawer> override;

  auto GetCouplerPercentageIndex [[nodiscard]] () -> int&;
  void SetCouplerPercentageIndex(int index);

 private:
  int coupler_percentage_index_{};
};

class CouplerNodeDrawer : public INodeDrawer {
 public:
  explicit CouplerNodeDrawer(std::shared_ptr<CouplerNode> node);

  auto GetLabel [[nodiscard]] () const -> std::string override;
  auto GetColor [[nodiscard]] () const -> ImColor override;

 private:
  std::shared_ptr<CouplerNode> node_{};
};
}  // namespace esc

#endif  // VH_CORE_COUPLER_NODE_H_
