#include "main.h"
#include "spi_74hc595.h"

uint8_t test_data = 0b0001110;

void SystemClock_Config(void);

int main(void){
	SystemClock_Config();
	spi_gpio_init();
 	spi1_config();

 	uint8_t result = hc595_loopback_check(test_data);


 	// Simple error checking feedback with a led.
 	if (result == 0x01) {
 	        while(1) {
 	        	hc595_write_byte(0x01); }
 	    }
 	    else if (result == 0x00) {
 	        while(1) {
 	            hc595_write_byte(0xFF);
 	            simple_delay(500000);
 	            hc595_write_byte(0x00);
 	            simple_delay(500000);
 	        }
 	    }
 	    else {
 	        while(1) {
 	            hc595_write_byte(0xFF);
 	            simple_delay(50000);
 	            hc595_write_byte(0x00);
 	            simple_delay(50000);
 	        }
 	    }

 	/*
	while(1){

		hc595_write_byte(0x01);
		simple_delay(2000000);
		hc595_write_byte(0x00);
		simple_delay(2000000);
	}

	*/
}



