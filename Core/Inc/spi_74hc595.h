#ifndef SPI_H_
#define SRC_H_

#include "stm32f103xb.h"
#include "stdint.h"

void spi_gpio_init(void);
void spi1_config(void);
void spi1_transmit(uint8_t *data,uint32_t size);
void spi1_receive(uint8_t *data,uint32_t size);
void cs_enable(void);
void cs_disable(void);
void hc595_write_byte(uint8_t data);
void simple_delay(volatile uint32_t count);
uint8_t hc595_loopback_check(uint8_t test_pattern);



#endif /* SPI_H_ */
