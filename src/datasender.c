/*
 * datasender.c
 *
 *  Created on: 19. 1. 2017
 *      Author: Me
 */

#include <string.h>
#include "datasender.h"
#include "gsm.h"
#include "datastore.h"
#include "core.h"
#include "jsonconstructor.h"
#include "ringbuffer.h"

typedef enum
{
	ds_idle = 0,
	ds_init,
	ds_connect,
	ds_waitforconnect,
	ds_jsonbuild,
	ds_send,
	ds_reply,
	ds4,
	ds5,
}ds_t;


void datasender_proc()
{
	static ds_t ds = ds_idle;
	switch(ds)
	{
		case ds_idle:
			if (storeddatanum() > 20)
				{
					gsm_poweron();
					ds = ds_init;
				}
			break;

		case ds_init:
			if (gsm_status() == gsm_status_success)
				{
					ds = ds_connect;
					gsm_init();
				}
			break;

		case ds_connect:
			if ((gsm_status() == gsm_status_success) && (gsm_socketstate() == 0))
				{
					ds = ds_waitforconnect;
					gsm_gprsconnect();
				}
			break;

		case ds_waitforconnect:
			if (gsm_status() == gsm_status_success)
				{
					ds = ds_jsonbuild;
				}
			else if (gsm_status() == gsm_status_error)
				{
					ds = ds_connect;
					gsm_gprsdisconnect();
				}
			break;

		case ds_jsonbuild:
			{
				char * sbuff = gsm_getsendbuffer();
				sbuff[0] = 0;
				constructhead(sbuff, 10);
				while (1)
					{
						datasample a;
						storeddataload(&a);
						uint16_t lastbyte = strlen(sbuff);
						if ((lastbyte > 800) || (storeddatanum() == 0))
							{
								constructentry(sbuff + lastbyte, &a, 0);
								ds = ds_send;
								break;
							}
						else
							constructentry(sbuff + lastbyte, &a, 1);

					}
				constructtail(sbuff + strlen(sbuff));
			}
			break;

		case ds_send:
			gsm_senddata(0);
			ds = ds_reply;
			break;

		case ds_reply:
			{
				uint16_t len;
				if ((len = gsm_getrecvlen()) != 0)
					{
						char * recvbuff = gsm_getrecvbuffer();
						recvbuff[len] = 0;
						if (strstr(recvbuff, "OK") != NULL)
							{
								storeddataloadsuccess();
								if (storeddatanum())
									ds = ds_connect;
								else
									ds = ds4;
							}
					}
				else if (gsm_socketstate() == 0)
					{
						ds = ds_connect;
					}
			}
			break;

		case ds4:
			gsm_poweroff();
			ds = ds5;
			break;

		case ds5:
			if (gsm_status() == gsm_status_success)
				{
					ds = ds_idle;
				}
			break;
	}
}
