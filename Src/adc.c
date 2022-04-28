/*
 * File: adc.c
 * Author: 20976313
 *
 * Description: Analog to digital converter functions. Used to parse voltage and current.
 *
 */

#include "prog_def.h"

void parse_voltage(void);
void parse_current(void);

uint16_t currentArray[20];
uint8_t currentIndex = 0;

void parse_voltage(void){
	vol = 0;
	int m = 0;
	for (int i = 1; i <ADC_BUFFER_SIZE; i+=2){
	vol = vol + adc_dma_buffer[i];
	m++;
	}
	vol = (vol/m)/4095*3.3/0.1925;
}

void parse_current(){
	float temp = 0;
	for (int i = 0; i <ADC_BUFFER_SIZE; i+=2){
	temp = temp + adc_dma_buffer[i];
	}
	if (!(currentIndex >= 20))
		{
		currentArray[currentIndex] = 2*1000*(temp/ADC_BUFFER_SIZE/4095)*3.3/5.2;
		}
	currentIndex++;
}

void update_current_voltage(){ //this function is meant to be called every second when secflag is != 0
	int sum = 0;
	for (int i = 0; i<20; i++){
	sum += currentArray[i];
	}
	parse_voltage();
	cur = sum/20;
	currentIndex = 0;
}
