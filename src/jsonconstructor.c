/*
 * jsonconstructor.c
 *
 *  Created on: 15. 1. 2017
 *      Author: Me
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "jsonconstructor.h"

const char start[] = "{\"stationid\":%lu,\"vals\":[";
const char end[] = "]}";
const char entrystruct[] =
		"{\"timestamp\":\"20%02d-%02d-%02d-%02d:%02d:%02d\","
		"\"T\":%d.%02d,"
		"\"RH\":%d.%02d,"
		"\"P\":%d.%02d}";

void constructhead(char * str, uint32_t stationid)
{
	sprintf(str, start, stationid);
}

void constructentry(char * str, datasample * entry, uint8_t notlast)
{
	int t = (int)(rawtotemp(entry->temp)*100);
	int rh = (int)(rawtorh(entry->humidity)*100);
	int p = (int)(rawtopressure(entry->pressure)*100);
	sprintf(str, entrystruct,
					entry->date.RTC_Year,
					entry->date.RTC_Month,
					entry->date.RTC_Date,
					entry->time.RTC_Hours,
					entry->time.RTC_Minutes,
					entry->time.RTC_Seconds,
					t/100, t%100,
					rh/100, rh%100,
					p/100, p%100);
	if (notlast)
		strcat(str,",");
}

void constructtail(char * str)
{
	strcpy(str, end);
}
