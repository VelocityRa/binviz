#pragma once

namespace stx {

template <typename T>
constexpr bool has_single_bit(T x) noexcept {
    return x != 0 && (x & (x - 1)) == 0;
}

template <typename T, std::size_t N, std::size_t... I>
constexpr auto create_array_impl(std::index_sequence<I...>) {
    return std::array<T, N>{ { I... } };
}

template <typename T, std::size_t N>
constexpr auto create_array() {
    return create_array_impl<T, N>(std::make_index_sequence<N>{});
}

// Returns the amount that should be added to 'value' to align it to 'alignment'
// 'alignment' must be a power of 2
template <typename T>
constexpr T align_offset(const T value, size_t alignment) {
    assert(has_single_bit(alignment));

    return static_cast<T>((~value + 1) & (alignment - 1));
}

// Returns the 'value' aligned to 'alignment'
// 'alignment' must be a power of 2
template <typename T>
constexpr T align(const T value, size_t alignment) {
    return value + align_offset(value, alignment);
}

}  // namespace stx