#ifndef VH_UI_SPLITTER_NODE_H_
#define VH_UI_SPLITTER_NODE_H_

#include <memory>

#include "core_splitter_node.h"
#include "ui_i_node_drawer.h"

namespace vh::esc::ui {
class SplitterNodeDrawer : public INodeDrawer {
 public:
  explicit SplitterNodeDrawer(std::shared_ptr<SplitterNode> node);

  auto GetName [[nodiscard]] () const -> std::string override;
  auto GetColor [[nodiscard]] () const -> ImColor override;

 private:
  auto GetNumOutputs [[nodiscard]] () const;

  std::shared_ptr<SplitterNode> node_{};
};
}  // namespace vh::esc::ui

#endif  // VH_UI_SPLITTER_NODE_H_
