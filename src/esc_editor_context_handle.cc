#include "esc_editor_context_handle.h"

#include "esc_cpp.h"

namespace esc {
EditorContextHandle::EditorContextHandle()
    : editor_context_{ne::CreateEditor()} {
  ne::SetCurrentEditor(editor_context_.get());
}

void EditorContextHandle::ContextDestroyer::operator()(
    ne::EditorContext* context) noexcept {
  cpp::Expects(context != nullptr);
  ne::DestroyEditor(context);
}
}  // namespace esc