#include <cstdint>

namespace stm32f446
{
namespace common
{

constexpr std::uint32_t bit(std::uint8_t position)
{
	return (1 << position);
}

constexpr std::uint32_t mask(std::uint8_t width)
{
	return ((1 << width) - 1);
}

constexpr std::uint32_t fieldMask(std::uint8_t width, std::uint8_t shift)
{
	return (mask(width) << shift);
}

constexpr bool isBitSet(std::uint32_t value, std::uint32_t bitMask)
{
	return ((value & bitMask) != 0);
}

constexpr bool isBitClear(std::uint32_t value, std::uint32_t bitMask)
{
	return ((value & bitMask) == 0);
}

constexpr std::uint32_t extractField(std::uint32_t value, std::uint32_t fieldMask, std::uint8_t shift)
{

}

}	// namespace common
}	// namespace stm32f446
