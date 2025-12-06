/*
 * SPI_74HC595.c
 *
 *  Created on: Dec 6, 2025
 *      Author: MiamiLights
 */

#include "spi_74hc595.h"

#define SPI1EN (1U<<12)
#define GPIOAEN (1U<<2)
#define SR_TXE (1U<<1)
#define SR_RXNE (1U<<0)
#define SR_BSY (1U<<7)

#define LATCH_LOW() GPIOA -> BSRR = (1U<<(9+16)) // Reset (Low) PA9
#define LATCH_HIGH() GPIOA -> BSRR = (1U<<9) // Set (High) PA9

void spi_gpio_init(){

	RCC -> APB2ENR |= GPIOAEN;

	// PA5, PA6, PA7 in alternate function
	GPIOA -> CRL &= ~(0xFU<<20);
	GPIOA -> CRL |= (0xBU<<20);

	GPIOA -> CRL &= ~(0xFU<<24);
	GPIOA -> CRL |= (0x4U<<24);

	GPIOA -> CRL &= ~(0xFU<<28);
	GPIOA -> CRL |= (0xBU<<28);

	// Set PA9 as SS
	GPIOA -> CRH &= ~(0xFU<<4);
	GPIOA -> CRH |= (0x3U<<4);

	// PA9 High default
	GPIOA -> BSRR = (1U<<9);
}

void spi1_config(void){

	RCC -> APB2ENR |= SPI1EN; // Clock activation for SPI

	SPI1 -> CR1 &= ~(0x7U<<3); // Set baud rate (clk / 4)
	SPI1 -> CR1 |= (3U<<3);

	SPI1 -> CR1 &= ~(1U<<0); // CPHA=0
	SPI1 -> CR1 &= ~(1U<<1); // CPHA=0
	// SPI1 -> CR1 |= (1U<<0) | (1U<<1); // CPOL=1, CPHA=1

	SPI1 -> CR1 |= (1U<<2); //Master mode

	SPI1 -> CR1 &= ~(1U<<11);//Data Frame Format DFF set to 8 bit.

	SPI1-> CR1 &= ~(1U<<7);//MSB first

	SPI1->CR1 |= (1U<<8) | (1U<<9); // SSM=1, SSi=1 -> Software Slave Management

	SPI1->CR1 |= (1U<<6); //SPI activation

}

void spi1_transmit(uint8_t *data, uint32_t size){

	/*
	 * 1. Clear overrun flag by reading DR and SR
	 * 2. Wait for TXE to signal empty DR
	 * 3. Write data to DR
	 * 4. Wait for busy flag to reset
	 */

	uint32_t i = 0;

	(void)SPI1->DR;
	(void)SPI1->SR;

	while(i < size){

		// wait for TXE bit to set -> This will indicate that the buffer is empty
		while(!(SPI1 -> SR & SR_TXE)){}
		SPI1 -> DR = data[i];

		while(!(SPI1 -> SR & SR_RXNE)){}
		(void)SPI1 -> DR;
		i++;

	}

	/* From datasheet: During discontinuous communications, there is a 2 APB clock period delay between the
	 * write operation to SPI_DR and the BSY bit setting. As a consequence, in transmit-only
	 * mode, it is mandatory to wait first until TXE is set and then until BSY is cleared after writing
	 * the last data.
	 */
	while(!(SPI1->SR & SR_TXE)){};
	while((SPI1->SR & SR_BSY)){};
}

void spi1_receive(uint8_t *data, uint32_t size){

	/*
	 * Before receiving data we must send a dummy byte to the device. When the device receives the dummy byte
	 * it transmits one byte of data. This will set RXNE flag to 1 (not empty). This signals the STM32 that
	 * there is some data in the Data Register ready to be read.
	 * For last we can copy the data from DR into our buffer. This clears the RXNE bit.
	 */

	while(size){
		// we wait for BSY bit to Reset, this indicates that SPI is not busy in communication
		while(!(SPI1 -> SR & SR_TXE)){};
		SPI1 -> DR = 0; //send dummy data
		while(!(SPI1 -> SR & SR_RXNE)){} //wait for data to arrive
		*data++ = (SPI1 -> DR);
		size--;
	}
}

void cs_enable(void){
	LATCH_LOW();
}

void cs_disable(void){
	LATCH_HIGH();
}

void hc595_write_byte(uint8_t data){
	cs_enable();
	spi1_transmit(&data, 1);
	simple_delay(100);
	cs_disable();
	simple_delay(100);
}

void SystemClock_Config(void){

}

void simple_delay(volatile uint32_t count){
	while(count --) __NOP();
}
