#pragma once

#include "stm32f446xx.h"
#include <cstdlib>

namespace board{
	inline constexpr GPIO_TypeDef* LedPort = GPIOA;
	inline constexpr std::uint8_t  LedPin  = 5;

	inline constexpr GPIO_TypeDef* ButtonPort = GPIOC;
	inline constexpr std::uint8_t  ButtonPin  = 13;

	inline constexpr GPIO_TypeDef* UartTxPort = GPIOA;
	inline constexpr std::uint8_t  UartTxPin  = 2;

	inline constexpr GPIO_TypeDef* UartRxPort = GPIOA;
	inline constexpr std::uint8_t  UartRxPin  = 3;
};
