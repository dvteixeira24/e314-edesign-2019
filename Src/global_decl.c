/*
 * File: global_decl.c
 * Author: 20976313
 *
 * Description: The initialization of global variables.
 *
 */

#include "prog_def.h"

uint16_t time;
uint8_t UTChh;
uint8_t UTCmm;
uint8_t UTCss;
float latitude;
float longitude;
float altitude;
float vol;
float cur;
float tmp;
float prs;
float hum;
float accx;
float accy;
float accz;

volatile uint8_t secFlag;
volatile uint8_t burnStatus;

uint8_t burnTimer;
uint8_t boundaryCnt;  //(max range of 5 on this one)

uint8_t rxByte;
uint8_t rxCnt;

char bufferOut[BUFFER_SIZE];
char bufferIn[BUFFER_SIZE];
volatile uint32_t adc_dma_buffer[ADC_BUFFER_SIZE];
char gfxBuffer[16];
char mathBuffer[11];

struct bme280_dev dev;
struct bme280_data comp_data;
lis2dh12_ctx_t dev_ctx;
