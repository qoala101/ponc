#ifndef VH_ESC_EDITOR_CONTEXT_HANDLE_H_
#define VH_ESC_EDITOR_CONTEXT_HANDLE_H_

#include <imgui_node_editor.h>

#include <memory>

namespace ne = ax::NodeEditor;

namespace esc {
class EditorContextHandle {
 public:
  EditorContextHandle();

 private:
  struct ContextDestroyer {
    void operator()(ne::EditorContext* context) noexcept;
  };

  std::unique_ptr<ne::EditorContext, ContextDestroyer> editor_context_{};
};
}  // namespace esc

#endif  // VH_ESC_EDITOR_CONTEXT_HANDLE_H_
