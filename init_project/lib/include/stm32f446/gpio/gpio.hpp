#pragma once

#include "stm32f446xx.h"
#include <cstdint>

namespace stm32f446
{
namespace gpio
{

enum class PinState : std::uint8_t
{
    LOW = 0,
    HIGH = 1
};

enum class Mode : std::uint8_t
{
    INPUT = 0,
    OUTPUT = 1,
    ALTERNATE = 2,
    ANALOG = 3
};

enum class OutputType : std::uint8_t
{
    PUSH_PULL = 0,
    OPEN_DRAIN = 1
};

enum class Speed : std::uint8_t
{
    LOW = 0,
    MEDIUM = 1,
    FAST = 2,
    HIGH = 3
};

enum class Pull : std::uint8_t
{
    NONE = 0,
    UP = 1,
    DOWN = 2
};

enum class InterruptEdge : std::uint8_t
{
    RISING,
    FALLING,
    RISING_FALLING
};

class Pin
{
public:
    constexpr Pin(GPIO_TypeDef* port, std::uint8_t pin) noexcept
        : port_(port), pin_(pin) {}

    void configureOutput(
        PinState initialState = PinState::LOW,
        Pull pull = Pull::NONE,
        OutputType outputType = OutputType::PUSH_PULL,
        Speed speed = Speed::LOW
    ) const;

    void configureInput(Pull pull = Pull::NONE) const;

    void configureAlternate(
        std::uint8_t alternateFunction,
        Pull pull = Pull::NONE,
        OutputType outputType = OutputType::PUSH_PULL,
        Speed speed = Speed::LOW
    ) const;

    void configureAnalog() const;

    void configureInterrupt(
        InterruptEdge edge = InterruptEdge::FALLING,
        Pull pull = Pull::NONE,
        bool enableNvic = true
    ) const;

    void write(PinState state) const;
    void set() const;
    void reset() const;
    void toggle() const;

    PinState read() const;
    PinState outputState() const;
    bool isHigh() const;
    bool isLow() const;

    GPIO_TypeDef* port() const noexcept;
    std::uint8_t pin() const noexcept;
    std::uint32_t mask() const noexcept;

private:
    GPIO_TypeDef* port_;
    std::uint8_t pin_;
};

} // namespace gpio
} // namespace stm32f446
