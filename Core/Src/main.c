#include "main.h"
#include "spi_74hc595.h"

void SystemClock_Config(void);

int main(void){
	SystemClock_Config();
	spi_gpio_init();
 	spi1_config();


	while(1){

		hc595_write_byte(0x01);
		simple_delay(2000000);
		hc595_write_byte(0x00);
		simple_delay(2000000);
	}

}



