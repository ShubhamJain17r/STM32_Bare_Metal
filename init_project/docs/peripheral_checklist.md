# Peripheral Implementation Checklist

Use this checklist whenever you add a new STM32F446 peripheral driver.

The goal is to keep every driver simple, consistent, readable, and close to the STM32F446 reference manual.

## 1. Peripheral Identity

Peripheral name:

```text
Example: GPIO, RCC, USART, SPI, I2C, TIM, ADC
```

Reference manual chapter:

```text
Example: Chapter 8 GPIO, Chapter 6 RCC, Chapter 19 USART
```

Library folder:

```text
lib/include/stm32f446/<peripheral>/
lib/src/<peripheral>/
```

Main files:

```text
<Peripheral>.hpp
<Peripheral>.cpp
```

Namespace:

```cpp
namespace stm32f446::<peripheral> {
}
```

Example:

```cpp
namespace stm32f446::gpio {
}
```

## 2. Required Registers

List every register block the driver touches.

Example for GPIO:

```text
RCC
GPIOx
SYSCFG
EXTI
NVIC
```

Example for USART:

```text
RCC
GPIOx
USARTx
NVIC
```

Example for SPI:

```text
RCC
GPIOx
SPIx
NVIC
```

For each register block, note why it is needed.

```text
RCC    -> enable peripheral clock
GPIOx  -> configure pins
USARTx -> configure and use serial peripheral
NVIC   -> enable interrupts, if used
```

## 3. Clock Requirements

Find which bus the peripheral belongs to.

```text
AHB1
AHB2
APB1
APB2
```

Find the RCC enable register and bit.

Examples:

```text
GPIOA  -> RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN
USART2 -> RCC->APB1ENR, RCC_APB1ENR_USART2EN
USART1 -> RCC->APB2ENR, RCC_APB2ENR_USART1EN
SPI1   -> RCC->APB2ENR, RCC_APB2ENR_SPI1EN
SPI2   -> RCC->APB1ENR, RCC_APB1ENR_SPI2EN
```

Decide whether the driver should enable its own clock.

Beginner recommendation:

```text
Yes. Each configure/init function should enable the needed peripheral clock.
```

Also decide whether the driver needs the bus frequency.

Examples:

```text
GPIO  -> usually no
UART  -> yes, for baud rate
I2C   -> yes, for timing
TIM   -> yes, for period/frequency
SPI   -> yes, for prescaler selection
ADC   -> maybe, depending on sampling setup
```

## 4. GPIO Pin Requirements

Does this peripheral need GPIO pins?

```text
Yes / No
```

If yes, list every pin and its alternate function.

Example:

```text
USART2_TX -> PA2, AF7
USART2_RX -> PA3, AF7

SPI1_SCK  -> PA5, AF5
SPI1_MISO -> PA6, AF5
SPI1_MOSI -> PA7, AF5
```

For each pin, note:

```text
Mode: input / output / alternate / analog
Pull: none / pull-up / pull-down
Output type: push-pull / open-drain
Speed: low / medium / high / very high
Alternate function number
```

Important: GPIO setup should be explicit and easy to inspect.

## 5. Public API Shape

Decide what object represents the peripheral.

Examples:

```cpp
gpio::Pin led(GPIOA, 5);
uart::Uart serial(USART2);
spi::Spi bus(SPI1);
timer::Timer timer(TIM2);
adc::Adc adc(ADC1);
```

Constructor rule:

```text
Constructors should store identity only.
They should not configure hardware.
```

Good:

```cpp
uart::Uart serial(USART2);
serial.configure(config);
```

Avoid:

```cpp
uart::Uart serial(USART2, 115200); // secretly configures hardware
```

## 6. Configuration Design

List the settings the peripheral needs.

Example for UART:

```text
Baud rate
Data bits
Stop bits
Parity
Mode: TX, RX, TX/RX
```

Example for SPI:

```text
Mode: master/slave
Clock polarity
Clock phase
Bit order
Baud prescaler
Data size
Software/hardware chip select
```

Example for Timer:

```text
Prescaler
Auto-reload value
Counter mode
Clock division
```

Use `enum class` for fixed choices.

Example:

```cpp
enum class Parity {
    None,
    Even,
    Odd
};
```

Use a config struct when there are several options.

Good:

```cpp
struct Config {
    std::uint32_t baudRate;
    DataBits dataBits;
    StopBits stopBits;
    Parity parity;
};
```

Good API:

```cpp
serial.configure(config);
```

Avoid long parameter lists:

```cpp
serial.configure(115200, 8, 1, false, true, false);
```

## 7. Basic Operations

List the operations users will call most often.

Example for GPIO:

```cpp
set()
reset()
toggle()
read()
isHigh()
isLow()
```

Example for UART:

```cpp
writeByte()
readByte()
write()
read()
isTxEmpty()
isRxNotEmpty()
```

Example for SPI:

```cpp
transfer()
write()
read()
isBusy()
```

Example for Timer:

```cpp
start()
stop()
reset()
hasUpdated()
clearUpdateFlag()
```

Keep the first version small. Add more operations only when needed.

## 8. Blocking, Interrupt, And DMA Support

Decide which usage modes the first version supports.

Recommended order:

```text
1. Blocking / polling
2. Interrupt
3. DMA
```

For the first version of most drivers, support only blocking/polling mode.

Example:

```cpp
Status writeByte(std::uint8_t data, std::uint32_t timeout);
```

Add interrupt or DMA support later in separate files if the driver becomes large.

Possible future split:

```text
Uart.hpp
Uart.cpp
UartInterrupt.hpp
UartInterrupt.cpp
UartDma.hpp
UartDma.cpp
```

## 9. Status And Error Handling

List what can go wrong.

Examples:

```text
Timeout
Busy
Invalid configuration
Overrun error
Framing error
Parity error
Bus error
NACK
```

Use a common status enum.

Example:

```cpp
enum class Status {
    Ok,
    Error,
    Timeout,
    Busy,
    InvalidConfig
};
```

Use `Status` for operations that can fail.

Example:

```cpp
Status writeByte(std::uint8_t data, std::uint32_t timeout);
```

Use `void` for operations that should not fail.

Example:

```cpp
void gpio::Pin::set();
```

## 10. Timeout Rules

If a function waits for a hardware flag, it should usually have a timeout.

Example wait conditions:

```text
UART TXE flag
UART RXNE flag
SPI TXE flag
SPI RXNE flag
SPI BSY flag
I2C SB/ADDR/BTF flags
ADC EOC flag
```

Avoid accidental infinite loops:

```cpp
while (!(USART2->SR & USART_SR_TXE)) {
}
```

Better:

```cpp
while (!(USART2->SR & USART_SR_TXE)) {
    if (timeoutExpired()) {
        return Status::Timeout;
    }
}
```

For very early experiments, infinite waits are acceptable, but document them clearly.

## 11. Interrupt Support

If the peripheral uses interrupts, decide:

```text
Which IRQ number?
Which interrupt enable bit?
Which status flag triggers the interrupt?
How is the flag cleared?
What callback should be called?
```

Interrupt handlers must use C linkage:

```cpp
extern "C" void USART2_IRQHandler(void)
{
}
```

Keep interrupt callbacks short.

Avoid doing slow work inside interrupt handlers.

Good interrupt flow:

```text
IRQ handler
  -> check flag
  -> clear flag if needed
  -> update small state or call short callback
  -> exit
```

## 12. Register Flag Clearing

For every flag used, check how it is cleared.

Do not assume all flags clear the same way.

Examples:

```text
EXTI pending bit -> cleared by writing 1
Some UART flags  -> cleared by reading SR then DR
Timer update flag -> cleared by writing 0 to UIF
```

Write a note in the driver if clearing behavior is unusual.

Example:

```cpp
// EXTI pending bits are cleared by writing 1.
EXTI->PR = 1UL << line;
```

## 13. Private Helpers

Decide which helpers are private to one source file.

Private helpers go inside an unnamed namespace in the `.cpp` file:

```cpp
namespace {

void configureBaudRate()
{
}

}
```

Use private helpers for:

```text
register conversions
small calculations
internal wait functions
flag helpers used by one file only
```

If many peripherals need the helper, move it to:

```text
lib/include/stm32f446/common/
```

Examples of common helpers:

```text
bit()
setBit()
clearBit()
writeField()
setTwoBitField()
```

## 14. Public Header Cleanliness

Before finishing the `.hpp` file, check:

```text
Can a user understand the API quickly?
Are there too many includes?
Are implementation details hidden?
Are private helper functions absent?
Are register details exposed only when useful?
```

Public headers should show what the driver does, not all internal machinery.

## 15. Example Requirement

Every peripheral should eventually have one small example.

Examples:

```text
examples/gpio_blink
examples/uart_echo
examples/spi_loopback
examples/timer_delay
examples/pwm_led
examples/adc_read
```

Each example should include:

```text
README.md
main.cpp
```

Example README should mention:

```text
What it demonstrates
Which pins are used
Expected behavior
```

## 16. Minimum Bring-Up Test

Before calling a peripheral done, check:

```text
Project compiles
Startup reaches main()
Peripheral clock enable bit is set
Registers show expected values in debugger
Basic operation works on hardware
Example code is simple and readable
No board-specific details are inside lib/
```

## 17. Commit Checklist

Before committing:

```text
No build outputs committed
No .metadata folder committed
No machine-specific absolute paths added
Public headers are in lib/include
Implementations are in lib/src
Example still matches current API
README or docs updated if needed
```

## 18. Final Design Questions

Ask these before moving to the next peripheral:

```text
Is this driver easy to read with the reference manual open?
Is the public API small?
Are the register writes easy to trace?
Did I avoid unnecessary abstraction?
Are common helpers reused instead of duplicated?
Are private details hidden in .cpp files?
Can I write a simple example in less than 50 lines?
```