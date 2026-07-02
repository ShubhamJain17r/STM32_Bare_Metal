#pragma once

#include "stm32f446xx.h"
#include <cstdint>

namespace stm32f446
{
namespace rcc
{

namespace gpio
{
	void enableClock(GPIO_TypeDef *port);
	bool isClockEnabled(GPIO_TypeDef *port);
}	// namespace gpio

namespace syscfg
{
	void enableClock();
	bool isClockEnabled();
}	// namespace syscfg

}	// namespace rcc
}	// namespace stm32f446
