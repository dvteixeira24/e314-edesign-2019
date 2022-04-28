/*
 * File: lcd.c
 * Author: 20976313
 *
 * Description: LCD Controller c file. This c file contains functions
 * for the init of the pc1601 lcd component used to display temperature
 * and altitude.
 *
 */

#include "prog_def.h"
#include <stdio.h>


void LCD_init(void){
	HAL_GPIO_WritePin(RS_PORT, RS_PIN, 0);
	HAL_GPIO_WritePin(RNW_PORT, RNW_PIN, 0); //set RS and RNW to 0
	dwt_us_delay(15000); //wait at least 15 msec (i.e. 50msec) for vdd to rise to ~5v
	LCD_push_nibble(0x3);
	dwt_us_delay(5000); //wait at least 4.1msec
	LCD_push_nibble(0x3);
	dwt_us_delay(150); //wait at least 100usec
	LCD_push_nibble(0x3);
	dwt_us_delay(50);
	LCD_push_nibble(0x2);
	dwt_us_delay(50);
	LCD_push_cmd(0x28); //set entry mode
	dwt_us_delay(100);
	//now clear
	LCD_clear();
}

void LCD_push_nibble(uint8_t data){ // NB format: ...000 DB7 DB6 DB5 DB4
									//      e.g.  ...000  0  1   1    0
	HAL_GPIO_WritePin(DB4_PORT, DB4_PIN, data & (1<<0) ? 1 : 0); //db4
	HAL_GPIO_WritePin(DB5_PORT, DB5_PIN, data & (1<<1) ? 1 : 0); //db5
	HAL_GPIO_WritePin(DB6_PORT, DB6_PIN, data & (1<<2) ? 1 : 0); //db6
	HAL_GPIO_WritePin(DB7_PORT, DB7_PIN, data & (1<<3) ? 1 : 0); //db7
	LCD_E_cycle();
}

void LCD_push_cmd(uint8_t data){
	HAL_GPIO_WritePin(RS_PORT, RS_PIN, 0);
	LCD_push_nibble(data>>4); //have to send in 4 bit pieces
	LCD_push_nibble(data);
	dwt_us_delay(40); //it takes roughly 40usec to complete commands usually
}

void LCD_E_cycle(void){
	HAL_GPIO_WritePin(E_PORT, E_PIN, 1);
	dwt_us_delay(4);
	HAL_GPIO_WritePin(E_PORT, E_PIN, 0);
	dwt_us_delay(4);
}

void LCD_clear(void){
	LCD_push_cmd(0x0e);
	dwt_us_delay(5000); //4 ms to clear
}

void LCD_push_mem(uint8_t data){
	HAL_GPIO_WritePin(RS_PORT, RS_PIN, 1);
	LCD_push_nibble(data>>4);
	LCD_push_nibble(data);
	dwt_us_delay(50);
	HAL_GPIO_WritePin(RS_PORT, RS_PIN, 0);
}

void LCD_write_string(uint8_t* string) {
	uint8_t length = strlen((const char*)string);
    LCD_push_cmd(0x06);
    dwt_us_delay(50);////
    LCD_push_cmd(0x80);
    dwt_us_delay(50);
    for (int i = 0; i < length; i++)
    	{
        LCD_push_mem(string[i]);
        dwt_us_delay(100);
        if (i == 7){
            LCD_push_cmd(0xC0); //move to line2!
            dwt_us_delay(100);
        }
    }
    dwt_us_delay(150);
}

//context specific function

void fill_buffer_gfx(){
	//pretty horrible function to use at the moment, will rewrite at a later stage
	snprintf((char*)gfxBuffer,16,"%-12d%3d", (int)altitude, (int)tmp);
	strcat((char*)gfxBuffer, "C");
	int i = 0;
	int j = 1;
	while (j) {
	if 	(gfxBuffer[i] == ' '){
		gfxBuffer[i] = 'm';
		j = 0;
	}
	i++;
	}
	if (burnStatus == 1) gfxBuffer[9] = 'B';
}

