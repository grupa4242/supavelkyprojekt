/*
 * datastore.h
 *
 *  Created on: 24. 12. 2016
 *      Author: Me
 */

#ifndef DATASTORE_H_
#define DATASTORE_H_

#include <stdint.h>
#include "stm32l1xx.h"

typedef struct
{
	RTC_TimeTypeDef time;
	RTC_DateTypeDef date;
	uint16_t temp;
	uint16_t humidity;
	int16_t pressure;
} datasample;

void datastore_proc();
void datastore_collectdata();

uint32_t storeddatanum();
uint8_t storeddataload(datasample * ptr);
void storeddataloadsuccess();
void storeddataloadfail();

float rawtopressure(uint16_t raw);
float rawtotemp(uint16_t raw);
float rawtorh(uint16_t raw);

#define DATASTORELEN (1<<10)
#define SCANINTERVAL 10U

#endif /* DATASTORE_H_ */
