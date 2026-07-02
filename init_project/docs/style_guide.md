# STM32F446 Peripheral Library Style Guide

This guide defines the coding and design style for this STM32F446 bare-metal C++ peripheral library.

The goal is to keep the library simple, readable, consistent, and close to the STM32F446 reference manual.

## 1. Project Goals

This library should:

- Use C++ for cleaner structure.
- Use direct register access through `stm32f446xx.h`.
- Avoid STM32 HAL.
- Avoid unnecessary abstraction.
- Stay easy to debug with the reference manual open.
- Be beginner-friendly but still well organized.

This library should not try to support every STM32 family at first.

Target chip:

```text
STM32F446RETx
```

Device header:

```cpp
#include "stm32f446xx.h"
```

## 2. Folder Rules

Public headers go here:

```text
lib/include/stm32f446/
```

Source files go here:

```text
lib/src/
```

Board-specific files go here:

```text
boards/
```

Examples go here:

```text
examples/
```

Documentation goes here:

```text
docs/
```

Do not put board-specific pin names inside `lib/`.

Good:

```text
boards/nucleo_f446re/Board.hpp
```

Avoid:

```text
lib/include/stm32f446/gpio/BoardPins.hpp
```

## 3. File Naming

Use PascalCase for library files:

```text
Gpio.hpp
Gpio.cpp
Rcc.hpp
Rcc.cpp
RegisterUtils.hpp
Error.hpp
```

Use matching names for header/source pairs:

```text
lib/include/stm32f446/gpio/Gpio.hpp
lib/src/gpio/Gpio.cpp
```

Use lowercase folder names:

```text
gpio/
rcc/
uart/
spi/
i2c/
timer/
adc/
common/
```

## 4. Header And Source Split

Header files should contain:

- public classes
- public enums
- public structs
- function declarations
- small `constexpr` helpers
- small inline functions when truly useful

Source files should contain:

- register manipulation logic
- larger function definitions
- private helper functions
- implementation details

Good header:

```cpp
class Pin {
public:
    Pin(GPIO_TypeDef* port, std::uint8_t pin);

    void configureOutput();
    void set();
    void reset();

private:
    GPIO_TypeDef* port_;
    std::uint8_t pin_;
};
```

Good source:

```cpp
void Pin::set()
{
    port_->BSRR = 1UL << pin_;
}
```

Avoid putting large register configuration functions directly in headers unless there is a clear reason.

## 5. Namespace Rules

Use one top-level namespace:

```cpp
namespace stm32f446 {
}
```

Each peripheral gets its own nested namespace:

```cpp
namespace stm32f446::gpio {
}

namespace stm32f446::rcc {
}

namespace stm32f446::uart {
}
```

Common utilities go in:

```cpp
namespace stm32f446::common {
}
```

Do not use this in header files:

```cpp
using namespace stm32f446;
```

It pollutes every file that includes the header.

It is okay to use local aliases in `.cpp` files or examples:

```cpp
using stm32f446::gpio::Pin;
```

## 6. Class Design Rules

Classes should represent hardware concepts.

Examples:

```cpp
gpio::Pin led(GPIOA, 5);
uart::Uart serial(USART2);
spi::Spi bus(SPI1);
timer::Timer timer(TIM2);
```

Constructors should only store identity.

Good:

```cpp
Pin led(GPIOA, 5);
led.configureOutput();
```

Avoid:

```cpp
Pin led(GPIOA, 5); // constructor secretly configures hardware
```

Hardware setup should happen through explicit functions:

```cpp
configure()
init()
configureOutput()
configureAlternate()
```

## 7. Public API Rules

Public APIs should be small, clear, and hard to misuse.

Good:

```cpp
led.configureOutput();
led.set();
led.reset();
led.toggle();
```

Avoid long unclear parameter lists:

```cpp
uart.configure(115200, 8, 1, false, true, false);
```

Prefer config structs when there are multiple settings:

```cpp
UartConfig config {
    .baudRate = 115200,
    .dataBits = DataBits::Eight,
    .stopBits = StopBits::One,
    .parity = Parity::None,
};

uart.configure(config);
```

Use `enum class` for fixed choices:

```cpp
enum class Pull {
    None,
    Up,
    Down
};
```

Avoid raw magic values in public APIs.

## 8. Register Access Rules

Use direct register access through CMSIS/device headers.

Good:

```cpp
RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
GPIOA->BSRR = 1UL << 5;
```

Do not use HAL functions:

```cpp
HAL_GPIO_WritePin(...);
HAL_UART_Transmit(...);
```

For GPIO APIs, use pin numbers:

```cpp
Pin led(GPIOA, 5);
```

Avoid HAL-style pin masks in your own API:

```cpp
Pin led(GPIOA, GPIO_PIN_5);
```

Pin numbers make register operations easier:

```text
MODER shift = pin * 2
BSRR set    = pin
BSRR reset  = pin + 16
AFR index   = pin / 8
AFR field   = pin % 8
```

## 9. Bit Manipulation Rules

Use helper functions for repeated bit operations.

Good:

```cpp
common::setBit(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);
common::setTwoBitField(GPIOA->MODER, pin, modeValue);
```

Avoid repeating complicated masks everywhere.

For GPIO output set/reset, prefer `BSRR`:

```cpp
GPIOA->BSRR = 1UL << 5;        // set PA5
GPIOA->BSRR = 1UL << (5 + 16); // reset PA5
```

Avoid using `ODR` for simple set/reset:

```cpp
GPIOA->ODR |= 1UL << 5;
```

`BSRR` is atomic and safer.

## 10. Clock Rules

Every peripheral must have its clock enabled before its registers are used.

Example:

```cpp
RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
(void)RCC->AHB1ENR;
```

The dummy read helps ensure the clock enable write has taken effect before accessing the peripheral.

Clock-related code should preferably live in the RCC driver:

```text
rcc/Rcc.hpp
rcc/Rcc.cpp
```

Peripheral drivers may call RCC helpers internally.

## 11. Error Handling Rules

Do not use exceptions.

Avoid:

```cpp
throw Error{};
```

Use simple status values:

```cpp
enum class Status {
    Ok,
    Error,
    Timeout,
    Busy,
    InvalidConfig
};
```

Use `Status` for functions that can fail:

```cpp
Status writeByte(std::uint8_t data, std::uint32_t timeout);
```

Use `void` for simple functions that should not fail:

```cpp
void set();
void reset();
```

## 12. Memory Rules

Avoid dynamic allocation.

Do not use:

```cpp
new
delete
malloc
free
```

Avoid standard containers for now:

```cpp
std::vector
std::string
std::map
```

Prefer fixed-size storage:

```cpp
Callback callbacks[16] {};
```

This keeps memory usage predictable.

## 13. Interrupt Rules

Interrupt handlers must use C linkage:

```cpp
extern "C" void EXTI15_10_IRQHandler(void)
{
}
```

Keep interrupt handlers short.

Good interrupt flow:

```text
check flag
clear flag
store data or call a short callback
exit
```

Avoid slow work inside interrupts:

```text
long loops
printing large messages
blocking waits
complex calculations
```

If interrupt support becomes large, split it into separate files:

```text
UartInterrupt.hpp
UartInterrupt.cpp
```

## 14. Flag Clearing Rules

Always check the reference manual for how a flag is cleared.

Different flags clear differently.

Examples:

```text
EXTI pending bit    -> write 1 to clear
TIM update flag     -> write 0 to clear
Some USART errors   -> read SR, then read DR
```

Add comments for unusual flag behavior.

Good:

```cpp
// EXTI pending bits are cleared by writing 1.
EXTI->PR = 1UL << line;
```

## 15. Private Helper Rules

If a helper is used only in one `.cpp` file, put it in an unnamed namespace:

```cpp
namespace {

void configureBaudRate()
{
}

}
```

If a helper is shared by many peripherals, put it in:

```text
lib/include/stm32f446/common/
```

Rule of thumb:

```text
Used by one source file      -> unnamed namespace in .cpp
Used by many peripherals     -> common/
Needed by library users      -> public API
```

## 16. Include Rules

Include only what a file needs.

Most peripheral headers may need:

```cpp
#include "stm32f446xx.h"
#include <cstdint>
```

Avoid unnecessary includes in headers.

If possible, include extra dependencies in `.cpp` files instead of headers.

## 17. Comments

Use comments to explain why, not what.

Good:

```cpp
// Writing 1 clears the EXTI pending bit.
EXTI->PR = 1UL << line;
```

Avoid:

```cpp
// Set bit 5.
reg |= 1UL << 5;
```

The code already shows that.

## 18. Board Rules

Board files describe actual hardware connections.

Example:

```cpp
namespace board {
inline constexpr GPIO_TypeDef* LedPort = GPIOA;
inline constexpr std::uint8_t LedPin = 5;
}
```

Library files should not contain names like:

```cpp
LedPin
UserButton
DebugUart
```

Those belong in `boards/` or examples.

## 19. Example Rules

Examples should be small and focused.

Good examples:

```text
gpio_blink
uart_echo
timer_delay
pwm_led_fade
spi_loopback
adc_read
```

Each example should have:

```text
README.md
main.cpp
```

Each example should demonstrate one idea clearly.

## 20. Commit Rules

Before committing, check:

```text
No Debug/ or Release/ folders committed
No .metadata/ folder committed
No .elf, .bin, .hex, .map files committed
No machine-specific absolute paths added accidentally
Public headers are in lib/include
Implementations are in lib/src
Examples still match the current API
```

## 21. General Design Rule

Prefer simple and boring code.

A good driver should be understandable with:

```text
the source file open
the STM32F446 reference manual open
the debugger register view open
```

Do not add abstraction until it solves a real problem.