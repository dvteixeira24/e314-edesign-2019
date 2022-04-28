/*
 * File: uart.c
 * Author: 20976313
 *
 * Description: Handles functions relating the hardware uart serial
 * communcation channel. Both sending and receiving are handled here.
 * See gpgga.c for the handling of incoming gpgga messages.
 *
 */

#include "prog_def.h"
#include <stm32f3xx_hal.h>
#include <stm32f3xx_hal_uart.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

void push_to_buffer_out(char string[], int maxWrite);
void numeric_to_buffer(float number, char *buffer, int intsize, int floatsize, int filler);

void transmit(void) {

	clear_buffer(bufferOut, BUFFER_SIZE);
	bufferOut[0] = '$';
	push_to_buffer_out(STUDENT_NUMBER, 91);
	bufferOut[9] = ',';

	numeric_to_buffer(time, mathBuffer, 5, 0, ' ');
	push_to_buffer_out(mathBuffer, 91);
	bufferOut[15] = ',';

	numeric_to_buffer(UTChh, mathBuffer, 2, 0, '0');
	push_to_buffer_out(mathBuffer, 91);
	bufferOut[18] = ':';
	numeric_to_buffer(UTCmm, mathBuffer, 2, 0, '0');
	push_to_buffer_out(mathBuffer, 91);
	bufferOut[21] = ':';
	numeric_to_buffer(UTCss, mathBuffer, 2, 0, '0');
	push_to_buffer_out(mathBuffer, 91);
	bufferOut[24] = ',';

	numeric_to_buffer(tmp, mathBuffer, 3, 0, ' ');
	push_to_buffer_out(mathBuffer, 91);
	bufferOut[28] = ',';

	numeric_to_buffer(hum, mathBuffer, 3, 0, ' ');
	push_to_buffer_out(mathBuffer, 91);
	bufferOut[32] = ',';

	numeric_to_buffer(prs, mathBuffer, 3, 0, ' ');
	push_to_buffer_out(mathBuffer, 91);
	bufferOut[36] = ',';

	numeric_to_buffer(accx, mathBuffer, 4, 0, ' ');
	push_to_buffer_out(mathBuffer, 91);
	bufferOut[41] = ',';

	numeric_to_buffer(accy, mathBuffer, 4, 0, ' ');
	push_to_buffer_out(mathBuffer, 91);
	bufferOut[46] = ',';

	numeric_to_buffer(accz, mathBuffer, 4, 0, ' ');
	push_to_buffer_out(mathBuffer, 91);
	bufferOut[51] = ',';

	numeric_to_buffer(latitude, mathBuffer, 3, 6, ' ');
	push_to_buffer_out(mathBuffer, 91);
	bufferOut[62] = ',';

	numeric_to_buffer(longitude, mathBuffer, 4, 6, ' ');
	push_to_buffer_out(mathBuffer, 91);
	bufferOut[74] = ',';

	numeric_to_buffer(altitude, mathBuffer, 5, 1, ' ');
	push_to_buffer_out(mathBuffer, 91);
	bufferOut[82] = ',';

	numeric_to_buffer(cur, mathBuffer, 3, 0, ' ');
	push_to_buffer_out(mathBuffer, 91);
	bufferOut[86] = ',';

	numeric_to_buffer(vol, mathBuffer, 1, 1, ' ');
	push_to_buffer_out(mathBuffer, 91);
	bufferOut[90] = '\n';

	HAL_UART_Transmit(&huart1, (uint8_t*) bufferOut, 91, 100);

}

void put_char(char c) { //places a character in the input uart buffer
	if (c == '$') {
		clear_buffer(bufferIn, BUFFER_SIZE);
		rxCnt = 0;
	}
	if (rxCnt >= 91) //wrap around instead of writing into other memory
		rxCnt = 0;
	bufferIn[rxCnt] = c;
	if (rxByte == 10) {
		process_GPGGA();//end of incoming message signaled by \n escape character
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8,1);
		dwt_us_delay(5000);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8,0);
	}
	rxCnt++;
}

void clear_buffer(char string[], int size) {
	for (int i = 0; i<size; i++){
		string[i] = 0;
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	put_char((char) rxByte);
	if(HAL_UART_Receive_IT(huart, &rxByte, 1) == HAL_OK){
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 0);
	} else {
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, 1);
	}
	//green user led on nucleo will light up on failure to reprime
}

void push_to_buffer_out(char string[], int maxWrite){
	int len = strlen(string);
	int buffIndex = strlen(bufferOut);
	int i = buffIndex;
	int j = 0;
	while(i<buffIndex+len && j<len && i < maxWrite){
		bufferOut[i] = string[j];
		i++;
		j++;
	}
}

/*
 *  Permission to use the following code given by Anya Nutt 19933150@sun.ac.za
 */
void numeric_to_buffer(float number, char *buffer, int intsize, int floatsize, int filler) // filler is what to fill 'gaps' with eg. ' ' or '0'
{
	int integer;
	float decimal;
	uint8_t signFlag = 0; // 0 for pos, 1 for neg
	int i = intsize - 1; // primes i (buffer index) to start at last digit of integer precision

	if (number < 0) // negative number
	{
		signFlag = 1;
		number *= -1; // multiplication makes number positive
	}

	integer = (int)number;
	decimal = number - integer;

	clear_buffer(buffer, 11); // total size includes position for '.'


	// insert integer part of number

	// add elements into array from right to left
	do {
		buffer[i] = (integer % 10) + 48;
		integer /= 10;
		i--;
	} while (integer != 0);

	if (signFlag)
	{
		buffer[i] = '-';
		i--;
	}

	while (i >= 0)
	{
		buffer[i] = filler;
		i--;
	}

	if (floatsize) buffer[intsize] = '.';

	// insert decimal part of number
	for (int k = 0; k < floatsize; k++) // add to buffer from left to right
	{
		decimal *= 10;
		buffer[k + intsize + 1] = (int)decimal + 48;
		decimal = decimal - (int)decimal;
	}
}






