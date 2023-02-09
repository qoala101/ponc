#ifndef VH_CORE_SPLITTER_NODE_H_
#define VH_CORE_SPLITTER_NODE_H_

#include "core_node.h"
#include "esc_id_generator.h"

namespace esc {
// NOLINTNEXTLINE(*-multiple-inheritance)
class SplitterNode : public Node,
                     public std::enable_shared_from_this<SplitterNode> {
 public:
  explicit SplitterNode(esc::IdGenerator& id_generator, int n);

  auto CreateDrawer [[nodiscard]] ()
  -> std::unique_ptr<INodeDrawer> override;
};

class SplitterNodeDrawer : public INodeDrawer {
 public:
  explicit SplitterNodeDrawer(std::shared_ptr<SplitterNode> node);

  auto GetLabel [[nodiscard]] () const -> std::string override;
  auto GetColor [[nodiscard]] () const -> ImColor override;

 private:
  auto GetNumOutputs [[nodiscard]] () const;

  std::shared_ptr<SplitterNode> node_{};
};
}  // namespace esc

#endif  // VH_CORE_SPLITTER_NODE_H_
