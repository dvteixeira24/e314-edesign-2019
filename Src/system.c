/*
 * File: system.c
 * Author: 20976313
 *
 * Description: Basic functions such as initializing.
 *
 */

#include "prog_def.h"


void LCD_ready(void); //only called in init sys

void init_sys(){
	HAL_TIM_Base_Start_IT(&htim2);
	HAL_TIM_Base_Start_IT(&htim3);
	LCD_init();
	//LCD_ready();
	//DMA and ADC initialization
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_dma_buffer, ADC_BUFFER_SIZE);

	//reset burn pin and burn status as well as timers:
	HAL_GPIO_WritePin(BURN_PORT, BURN_PIN, 0);

	time = 0;
	burnStatus = 0;
	burnTimer = 0;

	//i2c
	bme_init();
	lis2dh21_init();

	HAL_UART_Receive_IT(&huart1, &rxByte, 1);
}

void system_loop(){
	if (secFlag){
		time++;
		update_current_voltage();
		bme_poll();
		lis2dh12_poll();
		fill_buffer_gfx();
		LCD_write_string((uint8_t *)gfxBuffer);
		transmit();
		secFlag = 0;
	}
	if (burnStatus == 1 && (time - burnTimer) == 10){
	burnStatus = 2;
	HAL_GPIO_WritePin(BURN_PORT, BURN_PIN, 1);
	}
}

void LCD_ready(){
	for (int i = 0; i<4; i++){
		LCD_write_string((uint8_t*)"ready_          ");
		dwt_us_delay(200000);
		LCD_write_string((uint8_t*)"ready           ");
		dwt_us_delay(200000);
	}

}

void system_cmd_handler(){

	uint8_t msgID = 255;

	if (strlen(bufferIn) < 16) msgID = 0;

	if((stringCompare(bufferIn, "$CMDIN_SOFTRESET", 16))) msgID = 1;
	if((stringCompare(bufferIn, "$CMDIN_BURNRESET", 16))) msgID = 2;
	if((stringCompare(bufferIn, "$CMDIN_BOARDINFO", 16))) msgID = 3;

	switch(msgID){
			case 0:
			LCD_write_string((uint8_t*)"Invalid cmd leng");
			dwt_us_delay(1000000);
			break;
			case 1:
			LCD_write_string((uint8_t*)"PERFORMING RESET");
			dwt_us_delay(1000000);
			init_sys();
			break;
			case 2:
			burnStatus = 0;
			boundaryCnt = 0;
			burnTimer = 0;
			LCD_write_string((uint8_t*)"BURNSTATUS RESET");
			dwt_us_delay(1000000);
			break;
			case 3:
			LCD_write_string((uint8_t*)"20976313 D TEIXE");
			dwt_us_delay(1000000);
			LCD_write_string((uint8_t*)"Design (E) 314  ");
			dwt_us_delay(1000000);
			LCD_write_string((uint8_t*)"HAB Controller  ");
			dwt_us_delay(1000000);
			break;

			default:
			LCD_write_string((uint8_t*)"Unknown command ");
			dwt_us_delay(500000);
			break;
	}

}
