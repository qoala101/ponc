#include "esc_editor_context_handle.h"

namespace esc {
EditorContextHandle::EditorContextHandle()
    : editor_context_{ne::CreateEditor()} {
  ne::SetCurrentEditor(editor_context_.get());
}

void EditorContextHandle::ContextDestroyer::operator()(
    ne::EditorContext* context) noexcept {
  ne::DestroyEditor(context);
}
}  // namespace esc