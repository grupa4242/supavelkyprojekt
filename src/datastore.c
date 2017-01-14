/*
 * datastore.c
 *
 *  Created on: 24. 12. 2016
 *      Author: Me
 */

#include "datastore.h"
#include "ads1100.h"
#include "sht21.h"


typedef struct
{
	uint32_t timestamp;
	uint32_t timestampdate;
	uint16_t temp;
	uint16_t humidity;
	uint16_t pressure;
} datasample;

typedef enum
{
	ADSIdle = 0,
	ADSMeasTrig,
	ADSWaitForMeas,
	ADSDone,
}ADS1100_state_t;

typedef enum
{
	SHTIdle = 0,
	SHTMeasTrig,
	SHTMeasT,
	SHTMeasRH,
	SHTDone,
}SHT21_state_t;

static datasample datalog[DATASTORELEN];

static ADS1100_state_t ads_state = ADSIdle;
static SHT21_state_t sht_state = SHTIdle;

void datastore_proc()
{
	uint16_t tmp;
	uint8_t tmpstat;

	static uint16_t temperature;
	static uint16_t relhumiditiy;
	static uint16_t pressure;

	switch(ads_state)
	{
		case ADSIdle:

			break;
		case ADSMeasTrig:
			ADS1100_StartConv();
			ads_state = ADSWaitForMeas;
			break;
		case ADSWaitForMeas:
			ADS1100_readDataStatus(&tmp, &tmpstat);
			if (tmpstat)
				{
					pressure = tmp;
					ads_state = ADSDone;
				}
			break;
		case ADSDone:
			break;
	}

	switch(sht_state)
	{
		case SHTIdle:

			break;
		case SHTMeasTrig:
			SHT21_trigMeasT();
			sht_state = SHTMeasT;
			break;
		case SHTMeasT:
			SHT21_readVal(&tmp,&tmpstat);
			if (tmpstat)
				{
					temperature = tmp;
					SHT21_trigMeasRH();
					sht_state = SHTMeasRH;
				}
			break;
		case SHTMeasRH:
			SHT21_readVal(&tmp,&tmpstat);
			if (tmpstat)
				{
					relhumiditiy = tmp;
					sht_state = SHTDone;
				}
			break;
		case SHTDone:

			break;
	}

	if ((sht_state == SHTDone) && (ads_state == ADSDone))
		{
			sht_state = SHTIdle;
			ads_state = ADSIdle;


		}
}

void datastore_storedata()
{
	if (ads_state != ADSIdle)
		return;
	if (sht_state != SHTIdle)
		return;

	ads_state = ADSMeasTrig;
	sht_state = SHTMeasTrig;
}
