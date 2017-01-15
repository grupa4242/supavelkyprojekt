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

void datastore_proc();
void datastore_storedata();

uint32_t storeddatanum();
uint8_t storeddataload(datasample * ptr);

inline float rawtopressure(uint16_t raw);
inline float rawtotemp(uint16_t raw);
inline float rawtorh(uint16_t raw);

typedef struct
{
	RTC_TimeTypeDef time;
	RTC_DateTypeDef date;
	uint16_t temp;
	uint16_t humidity;
	int16_t pressure;
} datasample;

#define DATASTORELEN (1<<10)
#define SCANINTERVAL 10U

#endif /* DATASTORE_H_ */
