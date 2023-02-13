#ifndef VH_CORE_I_FAMILY_H_
#define VH_CORE_I_FAMILY_H_

#include <memory>
#include <vector>

#include "core_i_node.h"
#include "core_id_generator.h"
#include "cpp_interface.h"
#include "imgui_node_editor.h"

namespace esc {
namespace json {
class INodeParser;
class IFamilyWriter;
}  // namespace json

namespace draw {
class IFamilyDrawer;
}  // namespace draw

namespace core {
// ---
class IFamily : public cpp::Interface {
 public:
  // ---
  virtual auto CreateNodeParser [[nodiscard]] ()
  -> std::unique_ptr<json::INodeParser> = 0;
  // ---
  virtual auto CreateWriter [[nodiscard]] ()
  -> std::unique_ptr<json::IFamilyWriter> = 0;
  // ---
  virtual auto CreateDrawer [[nodiscard]] ()
  -> std::unique_ptr<draw::IFamilyDrawer> = 0;

  // ---
  auto GetNodes [[nodiscard]] () const
      -> const std::vector<std::shared_ptr<INode>> &;
  // ---
  auto EmplaceNode [[nodiscard]] (IdGenerator &id_generator) -> INode &;
  // ---
  void EraseNode(ne::NodeId &id);

 protected:
  // ---
  explicit IFamily(std::vector<std::shared_ptr<core::INode>> nodes);

  // ---
  auto EmplaceNode [[nodiscard]] (std::shared_ptr<core::INode> node) -> INode &;

 private:
  // ---
  virtual auto CreateNode [[nodiscard]] (IdGenerator &id_generator)
  -> std::shared_ptr<INode> = 0;

  // ---
  std::vector<std::shared_ptr<INode>> nodes_{};
};
}  // namespace core
}  // namespace esc

#endif  // VH_CORE_I_FAMILY_H_
