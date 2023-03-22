#ifndef VH_PONC_CORE_CONCEPTS_H_
#define VH_PONC_CORE_CONCEPTS_H_

#include <concepts>
#include <cstdint>

namespace vh::ponc::core {
///
template <typename T>
concept Id = requires(T t) {
               { t.Get() } -> std::same_as<uintptr_t>;
             } && std::constructible_from<uintptr_t>;
}  // namespace vh::ponc::core

#endif  // VH_PONC_CORE_CONCEPTS_H_
