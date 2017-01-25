/*
 * gsm.h
 *
 *  Created on: 15. 1. 2017
 *      Author: Me
 */

#ifndef GSM_H_
#define GSM_H_
#include <stdint.h>

typedef enum
{
	gsm_status_success = 0,
	gsm_status_busy,
	gsm_status_error
}gsm_status_t;

void gsm_proc();
void gsm_poweron();
void gsm_poweroff();

void gsm_init();

gsm_status_t gsm_status();
void gsm_gprsconnect();
void gsm_gprsdisconnect();

char * gsm_getsendbuffer();
void gsm_senddata(uint16_t len);

uint16_t gsm_getrecvlen();
char * gsm_getrecvbuffer();

uint8_t gsm_socketstate();

#endif /* GSM_H_ */
