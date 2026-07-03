#include "stm32f446/rcc/Rcc.hpp"

namespace stm32f446
{
namespace rcc
{

namespace gpio
{

void enableClock(GPIO_TypeDef *port)
{
	switch(port)
	{
	case GPIOA:
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
		break;
	case GPIOB:
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
		break;
	case GPIOC:
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
		break;
	}
}

bool isClockEnabled(GPIO_TypeDef *port)
{
	switch(port)
	{
	case GPIOA:
		return (RCC->AHB1ENR & RCC_AHB1ENR_GPIOAEN);
	case GPIOB:
		return (RCC->AHB1ENR & RCC_AHB1ENR_GPIOBEN);
	case GPIOC:
		return (RCC->AHB1ENR & RCC_AHB1ENR_GPIOBEN);
	}
}

}	// namespace gpio

namespace syscfg
{

void enableClock()
{
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
}

bool isClockEnabled()
{
	return (RCC->APB2ENR & RCC_APB2ENR_SYSCFGEN);
}

}	// namespace syscfg

}	// namespace rcc
}	// namespace stm32f446
