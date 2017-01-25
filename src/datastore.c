/*
 * datastore.c
 *
 *  Created on: 24. 12. 2016
 *      Author: Me
 */

#include "datastore.h"
#include "ads1100.h"
#include "sht21.h"
#include "stm32l1xx.h"
#include "core.h"

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
static uint32_t writeptr = 0;
static uint32_t readptr = 0;
static uint32_t readptrbkp = 0;

void datastore_proc()
{
	uint8_t tmpstat;

	static uint16_t temperature;
	static uint16_t relhumiditiy;
	static int16_t pressure;

	static uint32_t lastrun = 0;

	if ((millis() - lastrun) < SCANINTERVAL)
		return;

	lastrun += SCANINTERVAL;

	switch(ads_state)
	{
		case ADSIdle:

			break;
		case ADSMeasTrig:
			ADS1100_StartConv();
			ads_state = ADSWaitForMeas;
			break;
		case ADSWaitForMeas:
			ADS1100_readDataStatus(&pressure, &tmpstat);
			if (tmpstat)
				{
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
			SHT21_readVal(&temperature,&tmpstat);
			if (tmpstat)
				{
					SHT21_trigMeasRH();
					sht_state = SHTMeasRH;
				}
			break;
		case SHTMeasRH:
			SHT21_readVal(&relhumiditiy,&tmpstat);
			if (tmpstat)
				{
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

			datasample * tmpdatalog = datalog + writeptr;

			tmpdatalog->humidity = relhumiditiy;
			tmpdatalog->pressure = pressure;
			tmpdatalog->temp = temperature;

			RTC_GetTime(RTC_Format_BIN, &(tmpdatalog->time));
			RTC_GetDate(RTC_Format_BIN, &(tmpdatalog->date));

			writeptr++;
			writeptr &= (DATASTORELEN - 1);

			if (readptr == writeptr)
				{
					readptr++;
					readptr &= (DATASTORELEN - 1);
				}
			if (readptr == writeptr)
				{
					readptrbkp++;
					readptrbkp &= (DATASTORELEN - 1);
				}
		}
}

void datastore_collectdata()
{
	if (ads_state != ADSIdle)
		return;
	if (sht_state != SHTIdle)
		return;

	ads_state = ADSMeasTrig;
	sht_state = SHTMeasTrig;
}

uint32_t storeddatanum()
{
	return (writeptr - readptr) & (DATASTORELEN - 1);
}

uint8_t storeddataload(datasample * ptr)
{
	if (writeptr == readptr)
		return 0;
	*ptr = datalog[readptr];
	readptr++;
	readptr &= (DATASTORELEN - 1);
	return 1;
}

void storeddataloadsuccess()
{
	readptrbkp = readptr;
}

void storeddataloadfail()
{
	readptr = readptrbkp;
}

#define _K (3 * (1L << 15) / 3.3)

float rawtopressure(uint16_t raw)
{
	return (raw + 0.095 * _K) / (0.009 * _K);
}

#undef _K

float rawtotemp(uint16_t raw)
{
	return SHT21_CalcTemperatureC(raw);
}

float rawtorh(uint16_t raw)
{
	return SHT21_CalcRH(raw);
}
