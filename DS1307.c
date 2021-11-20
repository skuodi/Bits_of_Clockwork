/*
 * DS1307.c
 *
 *  Created on: Nov 16, 2021
 *      Author: @skuodi
 *
 ****************************************************
 * NB:
 * 		Time setting calls for special care as this library provides no input verification.
 * 		Setting invalid time (e.g 70sec) leaves the RTC in an undefined state
 */

#include "DS1307.h"

#define DS1307_ADDR 0b11010000

/**
 * @brief 	This function takes DS1307 second/minutes bits and returns the decimal value
 * @param 	inTime:		 holds the bits to be converted to dec format
 * @retval 	finalTime:	 decimal value of time
 */
uint8_t getSeconds(uint8_t inTime) {
	uint8_t finalTime = 0;				//holds the return value
	inTime &= ~(1 << 7);					//ignore bit 7 by setting it to 0
	finalTime += ((inTime >> 4) * 10); //convert bits 6, 5 and 4 to decimal to get the tens place of numbers
	/*ignore the tens bits (bit 4, 5, 6) by setting them to 0*/
	for (int i = 4; i < 7; i++) {
		inTime &= ~(1 << i); 	//ignore bits 4, 5, and 6 by setting them to 0
	}
	finalTime += inTime;				//add ones place values of the time

	return finalTime;

}

/**

 @brief This function takes DS1307 hours register bits and determines whether time is in AM/PM or 24 hour
 @retval hours: returns A/P for 12-hour clock and H for 24 hour clock
 */
char getMeridian(void) {
	uint8_t inTime;
	HAL_I2C_Mem_Read(&hi2c2,  DS1307_ADDR, 0x02, 1, &inTime, 1, 100);
	char hours = 'x';				//holds return value, x = undefined/error
	if (inTime >> 6) {					//if 12-hour clock,
		inTime &= ~(1<<6);					//ignore bit 6
		if (inTime >> 5)					//and check if PM/AM
			hours = 'P';					//return PM
		else
			hours = 'A';					//return AM
	} else {
		hours = 'H';					//24-hour clock
	}
	return hours;
}
/**

 @brief This function takes DS1307 hours register bits and returns the decimal value
 @param 	inTime: bits to be converted to decimal value
 @retval 	hrs: 	decimal value of hours
 */

uint8_t getHours(uint8_t inTime) {
	uint8_t hrs = 0;				//return value

	if (inTime >> 6)					//if 12-hour clock,
		inTime &= 0b00011111;//ignore bits starting with bit 5 heading towards the MSB
	else
		inTime &= 0b00111111;//ignore bits starting with bit 6 heading towards the MSB

	hrs += (inTime >> 4) * 10;

	inTime &= 0b00001111;				//ignore 7,6,5,4 by setting them to 0

	hrs += inTime;
	return hrs;
}

/**
 * @brief 	This function takes DS1307 time(minute/day/date/month/year) register values and returns a decimal number
 * @param 	inTime:		 holds the bits to be converted to dec format
 * @retval 	finalTime:	 decimal value of time
 */
uint8_t getTime(uint8_t inTime) {
	uint8_t finalTime = 0;				//holds the return value
	finalTime += ((inTime >> 4) * 10); //convert bits 6, 5 and 4 to decimal to get the tens place of numbers

	/*ignore the tens bits (bit 4, 5, 6) by setting them to 0*/
	for (int i = 4; i < 7; i++) {
		inTime &= ~(1 << i); 	//ignore bits 4, 5, and 6 by setting them to 0
	}
	finalTime += inTime;				//add ones place values of the time

	return finalTime;

}

/**
 @brief This function takes an 8byte array and populates it with decimal values of time
 @param allTime: gets populated with time data [second,minute,hour,day,date,month,year,SQW]

 The square wave control byte (SQW) will be populated as-is from the DS1307*/
void readTime(uint8_t allTime[8]) {
	HAL_I2C_Mem_Read(&hi2c2,  DS1307_ADDR, 0, 1, allTime, 8, 100);
	allTime[0] = getSeconds(allTime[0]);
	allTime[1] = getTime(allTime[1]);
	allTime[2] = getHours(allTime[2]);
	allTime[3] = getTime(allTime[3]);
	allTime[4] = getTime(allTime[4]);
	allTime[5] = getTime(allTime[5]);
	allTime[6] = getTime(allTime[6]);
}

/**
 @brief set the seconds,minutes,hours,day,date, month,year and 12 hour/24hour state
 @param timeSet: holds decimal values of time to set [second,minute,hour,day,date,month,year,SQW]
 @param meridian: A/P for AM/PM and H for 24-hour clock
 */

void setTime(uint8_t timeSet[8], char meridian) {
	uint8_t newTime[8] = { 0 };

	for (int i = 0; i < 7; i++) {				//convert decimal value to DS1307-compatible register bits
		newTime[i] = (timeSet[i] / 10) << 4;		//set 10's bits to the tens value
		newTime[i] |= (timeSet[i] % 10);				//and the remaining set to the ones bits
	}

	if (meridian != 'H') {
		newTime[2] |= (1 << 6);						//activate 12-hour clock
		if (meridian == 'P')
			newTime[2] |= (1 << 5);
		else
			newTime[2] &= ~(1 << 5);
	}

	newTime[7]  = timeSet[7];					//pass forward SQW register settings

	HAL_I2C_Mem_Write(&hi2c2, DS1307_ADDR, 0x00, 1, newTime, 8, 5);


}
