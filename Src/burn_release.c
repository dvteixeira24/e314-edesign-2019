/*
 * File: burn_release.c
 * Author: 20976313
 *
 * Description: Only one function here, it checks if the HAB controller
 * is outside of the specified range
 *
 */

#include"prog_def.h"

void boundary_check(){
	if((longitude < 17.976343 || longitude > 18.9345) && (altitude > 10000)) {
		boundaryCnt++;
	} else {
		boundaryCnt = 0;
	}
	if (boundaryCnt >= 5 && burnStatus == 0) {
	burnStatus = 1;
	HAL_GPIO_WritePin(BURN_PORT, BURN_PIN, 1);
	burnTimer = time;
	}
}
