/**
 * PONC @link https://github.com/qoala101/ponc @endlink
 * @author Volodymyr Hromakov (4y5t6r@gmail.com)
 * @copyright Copyright (c) 2023, MIT License
 */

#ifndef VH_PONC_CPP_VECTOR_BOOL_H_
#define VH_PONC_CPP_VECTOR_BOOL_H_

#include <vector>

namespace vh::ponc::cpp {
///
enum class Bool : bool { kFalse = false, kTrue = true };

///
using VectorBool = std::vector<Bool>;

///
auto At(VectorBool &vector, int index) -> bool &;
}  // namespace vh::ponc::cpp

#endif  // VH_PONC_CPP_VECTOR_BOOL_H_
