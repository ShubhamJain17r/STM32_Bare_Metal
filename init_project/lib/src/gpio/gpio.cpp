#include "stm32f446/gpio/Gpio.hpp"
#include "stm32f446/rcc/Rcc.hpp"

#include "stm32f446/common/RegisterUtils.hpp"

namespace stm32f446
{
namespace gpio
{

namespace{

}	// Gpio.cpp namespace

GPIO_TypeDef* Pin::port() const noexcept
{
	return port_;
}
std::uint8_t Pin::pin() const noexcept
{
	return pin_;
}

void Pin::configureMode(Mode mode) const
{
	setTwoBitField(port()->MODER, pin(), mode);
}

void Pin::configureOutputType(OutputType type) const
{
	if (type == OutputType::PUSH_PULL)
	{
		resetPin(port()->OTYPER, pin());
	}
	else
	{
		setPin(port()->OTYPER, pin());
	}
}

void Pin::configurePull(Pull pull) const
{
	setTwoBitField(port()->PUPDR, pin(), pull);
}

void Pin::configureSpeed(Speed speed) const
{
	setTwoBitField(port()->OSPEEDR, pin(), speed);
}

PinState Pin::readIdr() const
{
	return (readPin(port()->IDR, pin()) ? (PinState::HIGH) : (PinState::LOW));
}

PinState Pin::readOdr() const
{
	return (readPin(port()->ODR, pin()) ? (PinState::HIGH) : (PinState::LOW));
}

void Pin::configureBsrr(PinState state) const
{
	if (state == PinState::HIGH)
	{
		setPin(port()->BSRR, pin());
	}
	else
	{
		setPin(port()->BSRR, pin() + 16);
	}
}

void Pin::configureOutput(
        Pull pull = Pull::NONE,
        OutputType outputType = OutputType::PUSH_PULL,
        Speed speed = Speed::LOW) const
{
	enablePortClock(port_);
	configurePull(pull);
	configureOutputType(outputType);
	configureSpeed(speed);
	configureMode(Mode::OUTPUT);
}

void Pin::configureInput(Pull pull = Pull::NONE) const
{
	enablePortClock(port_);
	configurePull(pull);
	configureMode(Mode::INPUT);
}

void Pin::configureAlternate(
	std::uint8_t alternateFunction,
	Pull pull = Pull::NONE,
	OutputType outputType = OutputType::PUSH_PULL,
	Speed speed = Speed::LOW) const
{
	enablePortClock(port_);
	configureAlternateFunction(alternateFunction);
	configurePull(pull);
	configureOutputType(outputType);
	configureSpeed(speed);
	configureMode(Mode::ALTERNATE);
}

void Pin::configureAnalog() const
{
	enablePortClock(port_);
	configurePull(Pull::NONE);
	configureMode(Mode::ANALOG);
}

void Pin::configureInterrupt(
	InterruptEdge edge = InterruptEdge::FALLING,
	Pull pull = Pull::NONE,
	bool enableNvic = true) const;

void Pin::write(PinState state) const
{
	configureBsrr(state);
}

void Pin::set() const
{
	write(PinState::HIGH);
}

void Pin::reset() const
{
	write(PinState::LOW);
}

void Pin::toggle() const
{
	if (outputState() == PinState::HIGH)
	{
		set();
	}
	else
	{
		reset();
	}
}

PinState Pin::read() const
{
	return readIdr();
}

PinState Pin::outputState() const
{
	return readOdr();
}

bool Pin::isHigh() const
{
	return (read() == PinState::HIGH);
}

bool Pin::isLow() const
{
	return (read() == PinState::LOW);
}

}	// namespace gpio
}	// namespace stm32f446
