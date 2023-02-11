#ifndef VH_CORE_I_NODE_FACTORY_H_
#define VH_CORE_I_NODE_FACTORY_H_

#include <memory>
#include <vector>

#include "core_i_node.h"
#include "core_id_generator.h"
#include "cpp_interface.h"
#include "imgui_node_editor.h"

namespace esc {
namespace json {
class INodeParser;
class INodeFactoryWriter;
}  // namespace json

namespace draw {
class INodeFactoryDrawer;
}  // namespace draw

namespace core {
// ---
class INodeFactory : public cpp::Interface {
 public:
  // ---
  virtual auto CreateNodeParser [[nodiscard]] ()
  -> std::unique_ptr<json::INodeParser> = 0;
  // ---
  virtual auto CreateWriter [[nodiscard]] ()
  -> std::unique_ptr<json::INodeFactoryWriter> = 0;
  // ---
  virtual auto CreateDrawer [[nodiscard]] ()
  -> std::unique_ptr<draw::INodeFactoryDrawer> = 0;

  // ---
  auto GetNodes [[nodiscard]] () const
      -> const std::vector<std::shared_ptr<INode>> &;
  // ---
  auto EmplaceNode [[nodiscard]] (IdGenerator &id_generator) -> INode &;
  // ---
  void EraseNode(ne::NodeId &id);

 protected:
  // ---
  explicit INodeFactory(std::vector<std::shared_ptr<core::INode>> nodes);

 private:
  // ---
  virtual auto CreateNode [[nodiscard]] (IdGenerator &id_generator)
  -> std::shared_ptr<INode> = 0;

  // ---
  std::vector<std::shared_ptr<INode>> nodes_{};
};
}  // namespace core
}  // namespace esc

#endif  // VH_CORE_I_NODE_FACTORY_H_
