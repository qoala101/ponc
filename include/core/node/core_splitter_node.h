#ifndef VH_CORE_SPLITTER_NODE_H_
#define VH_CORE_SPLITTER_NODE_H_

#include "core_node.h"
#include "esc_id_generator.h"

// NOLINTNEXTLINE(*-multiple-inheritance)
class SplitterNode : public Node,
                     public std::enable_shared_from_this<SplitterNode> {
 public:
  explicit SplitterNode(esc::IdGenerator& id_generator, int n);

  auto GetDrawer [[nodiscard]] ()
  -> std::unique_ptr<vh::esc::ui::INodeDrawer> override;
};

#endif  // VH_CORE_SPLITTER_NODE_H_
