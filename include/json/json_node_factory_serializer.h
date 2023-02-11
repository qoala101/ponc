#ifndef VH_JSON_NODE_FACTORY_SERIALIZER_H_
#define VH_JSON_NODE_FACTORY_SERIALIZER_H_

#include <memory>

#include "crude_json.h"

namespace esc {
namespace core {
class INodeFactory;
}  // namespace core

namespace json {
// ---
// NOLINTNEXTLINE(*-special-member-functions)
class INodeFactoryParser {
 public:
  virtual ~INodeFactoryParser() = default;

  // ---
  auto TryToParseFromJson [[nodiscard]] (const crude_json::value &json) const
      -> std::optional<std::shared_ptr<core::INodeFactory>>;

 protected:
  INodeFactoryParser() = default;

 private:
  // ---
  virtual auto GetTypeName [[nodiscard]] () const -> std::string = 0;
  // ---
  virtual auto ParseFromJson [[nodiscard]] (const crude_json::value &json) const
      -> std::shared_ptr<core::INodeFactory> = 0;
};

// ---
// NOLINTNEXTLINE(*-special-member-functions)
class INodeFactoryWriter {
 public:
  virtual ~INodeFactoryWriter() = default;

  // ---
  auto WriteToJson [[nodiscard]] (const core::INodeFactory &factory) const
      -> crude_json::value;

 protected:
  INodeFactoryWriter() = default;

 private:
  // ---
  virtual auto GetTypeName [[nodiscard]] () const -> std::string = 0;
  // ---
  virtual auto WriteToJson [[nodiscard]] () const -> crude_json::value = 0;
};
}  // namespace json
}  // namespace esc

#endif  // VH_JSON_NODE_FACTORY_SERIALIZER_H_
