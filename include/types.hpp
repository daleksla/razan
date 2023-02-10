#ifndef TYPES_HPP
#define TYPES_HPP
#pragma once

#include <string>
#include <cstddef>
#include <cstdint>

/**
 * @brief File stores type aliases
 * @author Salih MSA
 */

namespace razan {

	using msg_t = std::u8string ;

	using key_t = std::array<std::uint8_t, 32> ; // AES 256 (binary bits)

} // razan

#endif // TYPES_HPP
