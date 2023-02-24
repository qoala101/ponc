/**
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 */

#ifndef VH_CORE_I_FAMILY_H_
#define VH_CORE_I_FAMILY_H_

#include <memory>
#include <vector>

#include "core_family_id.h"
#include "core_i_node.h"
#include "core_id_generator.h"
#include "cpp_interface.h"

namespace esc {
namespace json {
class INodeParser;
class IFamilyWriter;
}  // namespace json

namespace coreui {
class IFamilyDrawer;
}  // namespace coreui

namespace core {
class IFamily : public cpp::Interface {
 public:
  virtual auto CreateNode(IdGenerator &id_generator)
      -> std::shared_ptr<INode> = 0;
  virtual auto CreateNodeParser() -> std::unique_ptr<json::INodeParser> = 0;
  virtual auto CreateWriter() -> std::unique_ptr<json::IFamilyWriter> = 0;
  virtual auto CreateDrawer() -> std::unique_ptr<coreui::IFamilyDrawer> = 0;

  auto GetId() const -> FamilyId;

 protected:
  explicit IFamily(FamilyId id);

 private:
  FamilyId id_{};
};

// auto IsChildOf  (ne::NodeId node_id, const IFamily &family)
// -> bool;
}  // namespace core
}  // namespace esc

#endif  // VH_CORE_I_FAMILY_H_
