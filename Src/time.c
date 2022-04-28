/*
 * File: time.c
 * Author: 20976313
 *
 * Description: A simple dwt based delay function is contained here.
 *
 */

#include "prog_def.h"
#include <stm32f3xx_hal.h>
#include <stdint.h>

void dwt_init(void) { //init DWT
	if (!(CoreDebug->DEMCR & CoreDebug_DEMCR_TRCENA_Msk)) {
		CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
		DWT->CYCCNT = 0;
		DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
	}
}

void dwt_us_delay(uint32_t del_us) { //microsecond delay
	uint32_t start = DWT->CYCCNT, ticks = del_us * (SystemCoreClock / 1000000); //use system clock ticks to keep time
	while (DWT->CYCCNT - start < ticks)
		; //pause for del_us amount of microseconds
}

