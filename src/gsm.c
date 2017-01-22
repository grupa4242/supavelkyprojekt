/*
 * gsm.c
 *
 *  Created on: 15. 1. 2017
 *      Author: Me
 */

#include "gsm.h"
#include "core.h"
#include "ringbuffer.h"
#include <string.h>
#include <stdio.h>

#define PWRKEYLO() GPIOB->ODR &= ~(1<<6)
#define PWRKEYHI() GPIOB->ODR |= (1<<6)

#define settimer() timer = millis()
#define gettimer (millis() - timer)
#define sendcmd(x) do{txc = txc_sd; cmdptr = x;}while(0)

#define RSTIMEOUT 300
#define CMDBUFFLEN 64

#define STARTUPTIMEOUT 2000


typedef enum
{
	gsm_idle = 0,
	gsm_startup1,
	gsm_startup2,
	gsm_startup3,
	gsm_startup4,
	gsm_shutdown1,
	gsm_shutdown2,
	gsm_shutdown3,
	gsm_gprsinit1,
	gsm_gprsinit2,
	gsm_gprsinit3,
	gsm_gprsinit4,
	gsm_gprsinit5,
	gsm_connect1,
	gsm_connect2,
	gsm_connect3,
	gsm_disconnect1,
	gsm_disconnect2,
	gsm_connectfail,
	gsm_senddata1,
	gsm_senddata2,
	gsm_senddata3,
	gsm_sendfail,
}gsm_state_t;

typedef enum
{
	txc_none = 0,
	txc_reading,
	txc_sd,
	txc_sd1,
	txc_sd2,
	txc_ipd1,
	txc_ipd2,
}txc_t;

typedef enum
{
	rx_none = 0,
	rx_cmdtrig,
	rx_cmdbuff,
	rx_datatrig,
	rx_databuff,
	rx_havers,
	rx_noterm,
	rx_toolong,
	rx_nors,
	rx_havedata,
	rx_dataincomplete,
}rx_control_t;

static gsm_state_t gsm_state;
static uint16_t txlen = 0;

static char sendbuffer[1460];
static char recvbuffer[1460];

static rx_control_t rxc = rx_none;

static uint16_t ipdlen = 0;
uint8_t socketopen = 0;

const uint8_t sat[] = "AT\r";
const uint8_t sinit1[] = "AT+QICSGP=1,\"internet\"\r";
const uint8_t sinit2[] = "AT+QIDNSIP=1\r";
const uint8_t sinit3[] = "AT+QIHEAD=1\r";
const uint8_t sinit4[] = "AT+QIPROMPT=0\r";

const uint8_t scon[] = "AT+QIOPEN=\"TCP\",\"me.404.mn\",8051\r";
const uint8_t sdisc[] = "AT+QICLOSE\r";
const uint8_t ssend[] = "AT+QISEND\r";

void gsm_proc()
{
	static uint32_t timer;
	
	static uint32_t rcstimer;
	static uint8_t rxcnt = 0;
	static uint16_t txcnt = 0;
	static txc_t txc = txc_none;
	
	static const uint8_t (* cmdptr);
	static uint8_t sendretries = 0;
	

	static char cmdbuff[CMDBUFFLEN + 1];

	uint8_t havechar = 0;

	switch (rxc)
	{
		case rx_none:
			if (!getrxfull())
				break;
		case rx_cmdtrig:
			rcstimer = millis();
			rxcnt = 0;
			rxc = rx_cmdbuff;
		case rx_cmdbuff:
			if (getrxfull())
				{
					havechar = 1;
					cmdbuff[rxcnt] = getrxbuff();
					rxcnt++;
					cmdbuff[rxcnt] = 0;
					if (rxcnt >= CMDBUFFLEN)
						{
							rxc = rx_toolong;
						}
				}
			else if (millis() - rcstimer > RSTIMEOUT)
				{
					if (rxcnt)
						rxc = rx_noterm;
					else
						rxc = rx_nors;
				}
			break;

		case rx_datatrig:
			rcstimer = millis();
			rxcnt = 0;
			rxc = rx_databuff;

		case rx_databuff:
			if (getrxfull())
				{
					recvbuffer[rxcnt] = getrxbuff();
					rxcnt++;
					if(rxcnt == ipdlen)
						{
							rxc = rx_havedata;
							rxcnt = 0;
						}
				}
			else if ((millis() - rcstimer) > (ipdlen * 50))
				{
					rxc = rx_dataincomplete;
					rxcnt = 0;
				}
			break;
		case rx_havers:
		case rx_noterm:
		case rx_toolong:
		case rx_nors:
		case rx_havedata:
		case rx_dataincomplete:
			break;
	}

	if (gsm_state != gsm_shutdown3)
		switch (txc)
		{
			case txc_none:
				if (!havechar)
					break;
				txc = txc_reading;
			case txc_reading:
				if (havechar)
					{
						havechar = 0;
						char * tmp;
						if ((tmp = strstr(cmdbuff, "\r\n")) != NULL)
							{
								*tmp = 0;
								rxc = rx_havers;
								txc = txc_none;
								break;
							}
						else if (strstr(cmdbuff, "IPD") != NULL)
							{
								txc = txc_ipd1;
							}
					}
				else if (rxc == rx_cmdbuff)
					return;
				else
					break;

			case txc_ipd1:
				if (havechar)
					{
						havechar = 0;
						char * tmp;
						if ((tmp = strchr(cmdbuff, ':')) != NULL)
							{
								* tmp = 0;
								sscanf(cmdbuff + 3,"%hu", & ipdlen);
								rxc = rx_datatrig;
								txc = txc_ipd2;
							}
					}
				return;
			case txc_ipd2:
				if (rxc == rx_havedata)
					{
						txc = txc_none;
					}
				break;

			case txc_sd:
				sendretries = 0;
				txc = txc_sd1;

			case txc_sd1:
				if (cmdptr == NULL)
					{
						txc = txc_none;
						break;
					}
				buffwrite(cmdptr, 0);
				rxc = rx_cmdtrig;
				txc = txc_sd2;
				return;

			case txc_sd2:
				if (havechar)
					{
						char * tmp;
						if((tmp = strchr(cmdbuff, '\r')) != NULL)
							{
								* tmp = 0;
								rxc = rx_havers;
								txc = txc_none;
								break;
							}
					}
				else if(rxc == rx_nors)
					{
						sendretries++;
						if (sendretries > 3)
							{
								txc = txc_none;
								break;
							}
						else
							txc = txc_sd1;
					}
				else if((rxc == rx_noterm) || (rxc == rx_toolong))
					break;

				return;
		}

	if (socketopen)
		{
			if (rxc == rx_havers)
				{
					if (strstr(cmdbuff,"CLOSED") != NULL)
						{
							socketopen = 0;
							rxc = rx_none;
						}
				}
		}

	switch(gsm_state)
	{
		case gsm_idle:
			break;

		case gsm_startup1:
			PWRKEYLO();
			settimer();
			gsm_state = gsm_startup2;
			break;

		case gsm_startup2:
			if (gettimer > 2000)
				{
					gsm_state = gsm_startup3;
					flushrx();
					rxc = rx_none;
					PWRKEYHI();
					rxcnt = 0;
					settimer();
				}
			break;

		case gsm_startup3:
			if (rxc == rx_havers)
				{
					rxc = rx_none;
					if(strstr(cmdbuff,"RDY") != NULL)
						{
							gsm_state = gsm_startup4;
							settimer();
						}

				}
			else if (gettimer > STARTUPTIMEOUT)
						{
							gsm_state = gsm_startup1;
						}
			break;

		case gsm_startup4:
			if (rxc == rx_havers)
				{
					rxc = rx_none;
					if(strstr(cmdbuff,"+QNITZ") != NULL)
						{
							RTC_TimeTypeDef time;
							RTC_DateTypeDef date;

							char * tmp = strchr(cmdbuff, '\"');
							int a;
							sscanf(tmp, "\"%d/%hhu/%hhu,%hhu:%hhu:%hhu",
										 &a,
										 &(date.RTC_Month),
										 &(date.RTC_Date),
										 &(time.RTC_Hours),
										 &(time.RTC_Minutes),
										 &(time.RTC_Seconds));
							date.RTC_Year = a;
							date.RTC_WeekDay = 1;
							time.RTC_H12 = 0;
							rtc_settimedate(&time, &date);
							gsm_state = gsm_idle;
						}
				}
			else if (gettimer > 75000)
				{
					gsm_state = gsm_startup1;
				}
			break;

		case gsm_shutdown1:
			sendcmd(sat);
			gsm_state = gsm_shutdown2;
			break;

		case gsm_shutdown2:
			if (rxc == rx_havers)
				{
					rxc = rx_none;
					PWRKEYLO();
					settimer();
					gsm_state = gsm_shutdown3;
				}
			else if (rxc == rx_nors)
				gsm_state = gsm_idle;
			break;

		case gsm_shutdown3:
			if (gettimer > 2000)
				{
					PWRKEYHI();
					gsm_state = gsm_idle;
					flushrx();
				}
			break;

		case gsm_gprsinit1:
			sendcmd(sinit1);
			gsm_state = gsm_gprsinit2;
			break;

		case gsm_gprsinit2:
			if (rxc == rx_havers)
				{
					rxc = rx_none;
					sendcmd(sinit2);
					gsm_state = gsm_gprsinit3;
				}
			break;

		case gsm_gprsinit3:
			if (rxc == rx_havers)
				{
					rxc = rx_none;
					sendcmd(sinit3);
					gsm_state = gsm_gprsinit4;
				}
			break;

		case gsm_gprsinit4:
			if (rxc == rx_havers)
				{
					rxc = rx_none;
					sendcmd(sinit4);
					gsm_state = gsm_gprsinit5;
				}
			break;

		case gsm_gprsinit5:
			if (rxc == rx_havers)
				{
					rxc = rx_none;
					gsm_state = gsm_idle;
				}
			break;

		case gsm_connect1:
			sendcmd(scon);
			gsm_state = gsm_connect2;
			break;

		case gsm_connect2:
			if (rxc == rx_havers)
				{
					rxc = rx_none;
					gsm_state = gsm_connect3;
					settimer();
				}
			break;

		case gsm_connect3:
			if (rxc == rx_havers)
				{
					if (cmdbuff[0] == '8')
						{
							rxc = rx_none;
							gsm_state = gsm_idle;
							socketopen = 1;
						}
					else
						gsm_state = gsm_connectfail;
				}
			else if (gettimer > 75000)
				{
					gsm_state = gsm_connectfail;
				}
			break;

		case gsm_disconnect1:
			if (socketopen == 1)
				{
					sendcmd(sdisc);
					gsm_state = gsm_disconnect2;
				}
			else
				gsm_state = gsm_idle;
			break;

		case gsm_disconnect2:
			if (socketopen == 0)
				{
					gsm_state = gsm_idle;
				}
			break;

		case gsm_senddata1:
			flushrx();
			rxc = rx_none;
			buffwrite(ssend, 0);
			txcnt = 0;
			gsm_state = gsm_senddata2;
			break;
		case gsm_senddata2:
			if((!gettxfull()) && (gettimer > 5))
				{
					settimer();
					if (txcnt >= txlen)
						{
							gsm_state = gsm_senddata3;
							puttxbuff(0x1A);
							settimer();
							break;
						}
					puttxbuff(sendbuffer[txcnt]);
					txcnt++;
				}
			break;
		case gsm_senddata3:
			if (rxc == rx_havers)
				{
					rxc = rx_none;
					if(strstr(cmdbuff, "OK") != NULL)
						{
							gsm_state = gsm_idle;
						}
				}
			else if (gettimer > 75000)
				{
					gsm_state = gsm_sendfail;
				}
			break;
		case gsm_connectfail:
			break;
		case gsm_sendfail:
			break;
	}
}

void gsm_poweron()
{
	gsm_state = gsm_startup1;
}

void gsm_poweroff()
{
	gsm_state = gsm_shutdown1;
}

gsm_status_t gsm_status()
{
	switch(gsm_state)
	{
		case gsm_idle:
			return gsm_status_success;
		case gsm_connectfail:
		case gsm_sendfail:
			return gsm_status_error;
		default:
			return gsm_status_busy;
	}
}

void gsm_init()
{
	gsm_state = gsm_gprsinit1;
}

void gsm_gprsconnect()
{
	gsm_state = gsm_connect1;
}

void gsm_gprsdisconnect()
{
	gsm_state = gsm_disconnect1;
}

char * gsm_getsendbuffer()
{
	return sendbuffer;
}

uint16_t gsm_getrecvlen()
{
	if (rxc == rx_havedata)
		return ipdlen;
	return 0;
}

char * gsm_getrecvbuffer()
{
	ipdlen = 0;
	rxc = rx_none;
	return recvbuffer;
}

void gsm_senddata(uint16_t len)
{
	if (len)
		txlen = len;
	else
		txlen = strlen(sendbuffer);
	gsm_state = gsm_senddata1;
}

uint8_t gsm_socketstate()
{
	return socketopen;
}
