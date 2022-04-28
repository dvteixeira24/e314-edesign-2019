/*
 * File: prog_def.h
 * Author: 20976313
 *
 * Description: Contains the definitions for many constants used
 * in various parts of the software
 *
 */

#include <stdint.h>
#include <string.h>
#include <stm32f3xx_hal.h>
#include "bme280.h"
#include "lis2dh12_reg.h"

#ifndef PROG_DEF_H_
#define PROG_DEF_H_

#define STUDENT_NUMBER		"20976313"
#define BUFFER_SIZE			92
#define ADC_BUFFER_SIZE		256
#define DB4_PORT			GPIOA
#define DB4_PIN				GPIO_PIN_11
#define DB5_PORT			GPIOA
#define DB5_PIN				GPIO_PIN_12
#define DB6_PORT			GPIOA
#define DB6_PIN				GPIO_PIN_10
#define DB7_PORT			GPIOB
#define DB7_PIN				GPIO_PIN_5
#define RS_PORT				GPIOC
#define RS_PIN				GPIO_PIN_8
#define RNW_PORT			GPIOC
#define RNW_PIN				GPIO_PIN_6
#define E_PORT				GPIOB
#define E_PIN				GPIO_PIN_3
#define BURN_PORT			GPIOB
#define BURN_PIN			GPIO_PIN_4
#define LED0_PORT			GPIOA
#define LED0_PIN			GPIO_PIN_5
#define ACC_MAX				999

extern uint16_t time;
extern uint8_t UTChh;
extern uint8_t UTCmm;
extern uint8_t UTCss;
extern float latitude;
extern float longitude;
extern float altitude;
extern float vol;
extern float cur;
extern float tmp;
extern float prs;
extern float hum;
extern float accx;
extern float accy;
extern float accz;

extern volatile uint8_t secFlag;
extern volatile uint8_t burnStatus;

extern uint8_t burnTimer;
extern uint8_t boundaryCnt;

extern uint8_t rxByte;
extern uint8_t rxCnt;

extern char bufferOut[BUFFER_SIZE];
extern char bufferIn[BUFFER_SIZE];
extern volatile uint32_t adc_dma_buffer[ADC_BUFFER_SIZE];
extern char gfxBuffer[16];
extern char mathBuffer[11];

extern UART_HandleTypeDef huart1; //note: declared in main.c
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern ADC_HandleTypeDef hadc1;
extern I2C_HandleTypeDef hi2c1;;

extern struct bme280_dev dev;
extern struct bme280_data comp_data;
extern lis2dh12_ctx_t dev_ctx;

//----------------------------------------------------------//

void system_loop(void);
void init_sys(void);
void transmit(void);

void dwt_init(void);
void dwt_us_delay(uint32_t del_us);

//lcd
void LCD_E_cycle(void);
void LCD_push_nibble(uint8_t data);
void LCD_push_cmd(uint8_t data);
void LCD_init(void);
void LCD_clear(void);
void LCD_write_string(uint8_t* string);
void LCD_push_mem(uint8_t data);
void fill_buffer_gfx(void);

//uart
void put_char(char c);
void clear_buffer(char string[], int size);

//gpgga
void process_GPGGA(void);
void boundary_check(void);

//extra thing i decided to implement
void system_cmd_handler(void);
int stringCompare(char str1[], char str2[], int len);

//current and volt sensing
void update_current_voltage(void);
void parse_current(void);

//i2c
void bme_init();
void bme_poll();
void lis2dh21_init();
void lis2dh12_poll();

#endif /* PROG_DEF_H_ */
