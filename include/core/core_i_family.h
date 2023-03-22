#ifndef VH_CORE_I_FAMILY_H_
#define VH_CORE_I_FAMILY_H_

#include <imgui_node_editor.h>

#include <memory>
#include <optional>
#include <vector>

#include "core_id_generator.h"
#include "cpp_interface.h"

namespace ne = ax::NodeEditor;

namespace esc {
namespace json {
///
class INodeParser;
///
class IFamilyWriter;
}  // namespace json

namespace coreui {
///
class IFamilyTraits;
}  // namespace coreui

namespace core {
///
class INode;

///
struct FamilyId : public ne::Details::SafePointerType<FamilyId> {
  ///
  using SafePointerType::SafePointerType;
};

///
enum class FamilyType { kFreePin };

///
class IFamily : public cpp::Interface {
 public:
  ///
  virtual auto GetType() const -> std::optional<FamilyType>;
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
  ///
  auto CreateSampleNode() const -> std::unique_ptr<INode>;

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
