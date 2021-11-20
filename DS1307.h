/*
 * DS1307.h
 *
 *  Created on: Nov 16, 2021
 *      Author: @skuodi
 *
 *      STM32 HAL library for DS1307 I2C RTC
 *
 */

#ifndef INC_DS1307_H_
#define INC_DS1307_H_

#include "stdint.h"                     //for declaration of uint8_t

#include "stm32f1xx_hal.h"                      //HAL library for the bluepill(STM32F103C8)
extern I2C_HandleTypeDef hi2c2;                 //handle for the i2c2 bus connected to my 24LC256

void readTime(uint8_t allTime[8]);
uint8_t getSeconds(uint8_t);
uint8_t getHours(uint8_t);
char getMeridian();
uint8_t getTime(uint8_t);
void setTime(uint8_t timeSet[8], char meridian);

#endif /* INC_DS1307_H_ */
