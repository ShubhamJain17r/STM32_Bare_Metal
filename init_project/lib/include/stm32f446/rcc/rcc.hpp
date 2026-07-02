#include "stm32f446xx.h"
#include <cstdint>

#include "stm32f446/common/Bit.hpp"
#include "stm32f446/common/RegisterUtils.hpp"

namespace stm32f446
{

namespace rcc
{

namespace gpio
{
	void enableClock(GPIO_TypeDef *port);
};	// namespace gpio

namespace syscfg
{
	void enableClock(void);
};	// namespace syscfg

};	// namespace rcc

};	// namespace stm32f446
