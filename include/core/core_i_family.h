/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

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

namespace coreui {
class IFamilyDrawer;
}  // namespace coreui

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
  -> std::unique_ptr<coreui::IFamilyDrawer> = 0;

  // ---
  auto GetNodes [[nodiscard]] () const
      -> const std::vector<std::shared_ptr<INode>> &;
  // ---
  auto FindNode [[nodiscard]] (ne::NodeId node_id) const
      -> const std::shared_ptr<INode> *;
  // ---
  auto EmplaceNode(IdGenerator &id_generator) -> const std::shared_ptr<INode> &;
  // ---
  void EraseNode(ne::NodeId node_id);

 protected:
  // ---
  explicit IFamily(std::vector<std::shared_ptr<core::INode>> nodes);

  // ---
  auto EmplaceNode(std::shared_ptr<core::INode> node)
      -> const std::shared_ptr<INode> &;

 private:
  // ---
  virtual auto CreateNode [[nodiscard]] (IdGenerator &id_generator)
  -> std::shared_ptr<INode> = 0;

  // ---
  std::vector<std::shared_ptr<INode>> nodes_{};
};

// ---
auto IsChildOf [[nodiscard]] (ne::NodeId node_id, const IFamily &family)
-> bool;
}  // namespace core
}  // namespace esc

#endif  // VH_CORE_I_FAMILY_H_
