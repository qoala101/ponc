/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_CORE_CONNECTION_H_
#define VH_PONC_CORE_CONNECTION_H_

#include <imgui.h>
#include <imgui_node_editor.h>

#include <string>

namespace ne = ax::NodeEditor;

namespace vh::ponc::core {
///
struct ConnectionId : public ne::Details::SafePointerType<ConnectionId> {
  ///
  using SafePointerType::SafePointerType;
};

///
struct Connection {
  ///
  ConnectionId id{};
  ///
  std::string name{};
  ///
  ImColor color{};
  ///
  float drop_per_length{};
  ///
  float drop_added{};
};

///
struct CustomConnection {
  ///
  ImColor color{};
  ///
  float drop_per_length{};
  ///
  float drop_added{};
};
}  // namespace vh::ponc::core

#endif  // VH_PONC_CORE_CONNECTION_H_
