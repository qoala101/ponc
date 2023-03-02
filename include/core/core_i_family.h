#ifndef VH_CORE_I_FAMILY_H_
#define VH_CORE_I_FAMILY_H_

#include <memory>
#include <vector>

#include "core_family_id.h"
#include "core_i_node.h"
#include "core_id_generator.h"
#include "cpp_interface.h"
#include "json_i_node_parser.h"

namespace esc {
namespace json {
///
class IFamilyWriter;
}  // namespace json

namespace coreui {
///
class IFamilyTraits;
}  // namespace coreui

namespace core {
///
class IFamily : public cpp::Interface {
 public:
  ///
  virtual auto CreateNode(IdGenerator &id_generator) const
      -> std::unique_ptr<INode> = 0;
  ///
  virtual auto CreateNodeParser() const
      -> std::unique_ptr<json::INodeParser> = 0;
  ///
  virtual auto CreateWriter() const -> std::unique_ptr<json::IFamilyWriter> = 0;
  ///
  virtual auto CreateUiTraits() const
      -> std::unique_ptr<coreui::IFamilyTraits> = 0;

  ///
  auto GetId() const -> FamilyId;

 protected:
  ///
  explicit IFamily(FamilyId id);

 private:
  ///
  FamilyId id_{};
};
}  // namespace core
}  // namespace esc

#endif  // VH_CORE_I_FAMILY_H_
