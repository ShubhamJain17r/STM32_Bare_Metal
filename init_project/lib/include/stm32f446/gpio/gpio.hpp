#include "stm32f446xx.h"
#include <cstdint>

#include "stm32f446/common/Bit.hpp"
#include "stm32f446/common/RegisterUtils.hpp"

namespace stm32f446
{

namespace gpio
{

enum class PinState : std::uint8_t
{
	LOW 		= 0,
	HIGH		= 1
}

enum class Mode : std::uint8_t
{
	INPUT 		= 0,
	OUTPUT		= 1,
	ALTERNATE	= 2,
	ANALOG		= 3
};

enum class OutputType : std::uint8_t
{
	PUSH_PULL 	= 0,
	OPEN_DRAIN	= 1
};

enum class Speed : std::uint8_t
{
	LOW 		= 0,
	MEDIUM		= 1,
	FAST		= 2,
	HIGH		= 3
};

enum class Pull : std::uint8_t
{
	NONE 		= 0,
	UP			= 1,
	DOWN		= 2
};

enum class InterruptEdge : std::uint8_t
{
	RISING,
	FALLING,
	RISING_FALLING
};

class Pin
{
private:
	GPIO_TypeDef*	_port;
	std::uint8_t 	_pin;

public:
	constexpr Pin(GPIO_TypeDef* port, std::uint8_t pin) noexcept
	:	_port(port), _pin(pin){}

	// configure GPIO mode
	void configureOutput(
			PinState initialState 	= PinState::LOW,
			Pull pull 				= Pull::NONE,
			OutputType outputType 	= OutputType::PUSH_PULL,
			Speed speed				= Speed::LOW
	) const;

	void configureInput(Pull pull = Pull::NONE) const;

	void configureAlternate(
			std::uint8_t alternateFunction,
			Pull pull 				= Pull::NONE,
			OutputType outputType 	= OutputType::PUSH_PULL,
			Speed speed				= Speed::LOW
	) const;

	void configureAnalog() const;

	// write bit
	void write(PinState state) const;
	void set(void) const;
	void reset(void) const;
	void toggle(void) const;

	// read bit
	PinState read(void) const;
	PinState outputState(void) const;
	bool isHigh(void) const;
	bool isLow(void) const;

	// getter
	GPIO_TypeDef* port(void) const noexcept;
	std::uint8_t pin(void) const noexcept;
	std::uint32_t mask(void) const noexcept;

	// interrupt
	void configureInterrupt(
			InterruptEdge edge 	= InterruptEdge::FALLING,
			Pull pull			= Pull::NONE,
			bool enableNVIC		= true
	) const;
};  // class Pin

};  // namespace gpio

};	// namespace stm32f446
