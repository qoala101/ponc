#ifndef VH_CORE_I_NODE_FACTORY_H_
#define VH_CORE_I_NODE_FACTORY_H_

#include <memory>

#include "core_id_generator.h"

namespace esc {
namespace json {
class INodeParser;
class INodeFactoryWriter;
}  // namespace json

namespace draw {
class INodeFactoryDrawer;
}  // namespace draw

namespace core {
class INode;

// ---
// NOLINTNEXTLINE(*-special-member-functions)
class INodeFactory {
 public:
  virtual ~INodeFactory() = default;

  // ---
  virtual auto CreateNode [[nodiscard]] (IdGenerator &id_generator)
  -> std::shared_ptr<INode> = 0;
  // ---
  virtual auto CreateNodeParser [[nodiscard]] ()
  -> std::unique_ptr<json::INodeParser> = 0;
  // ---
  virtual auto CreateWriter [[nodiscard]] ()
  -> std::unique_ptr<json::INodeFactoryWriter> = 0;
  // ---
  virtual auto CreateDrawer [[nodiscard]] ()
  -> std::unique_ptr<draw::INodeFactoryDrawer> = 0;

 protected:
  INodeFactory() = default;
};
}  // namespace core
}  // namespace esc

#endif  // VH_CORE_I_NODE_FACTORY_H_
