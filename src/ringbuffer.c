/*
 * ringbuffer.c
 *
 *  Created on: 25. 10. 2016
 *      Author: Lenka
 */

#include "ringbuffer.h"
#include "stm32l1xx.h"

#define TXCNTMSK (TXBUFFLEN - 1)

volatile char txbuff[TXBUFFLEN];
volatile uint16_t txrp = 0;
volatile uint16_t txwp = 0;


#define RXCNTMSK (RXBUFFLEN - 1)

volatile char rxbuff[RXBUFFLEN];
volatile uint16_t rxrp = 0;
volatile uint16_t rxwp = 0;


char gettxbuff()
{
	if (txwp == txrp)
		return 0;

	char tmp = txbuff[txrp];
	txrp++;
	txrp &= TXCNTMSK;
	return tmp;
}

void puttxbuff(char c)
{
	if (((txwp + 1) & TXCNTMSK) == txrp)
		return;

	txbuff[txwp] = c;
	txwp++;
	txwp &= TXCNTMSK;
	USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
}

uint16_t gettxfull()
{
	return (txwp - txrp) & TXCNTMSK;
}

uint16_t gettxfree()
{
	return ((txrp - txwp) & TXCNTMSK) - 1;
}

char getrxbuff()
{
	if (rxwp == rxrp)
		return 0;
	char tmp = rxbuff[rxrp];
	rxrp++;
	rxrp &= RXCNTMSK; 
	return tmp;
}

void putrxbuff(const char c)
{
	if (((rxwp + 1) & RXCNTMSK) == rxrp)
			return;

	rxbuff[rxwp] = c;
	rxwp++;
	rxwp &= RXCNTMSK;
}

uint16_t getrxfull()
{
	return (rxwp - rxrp) & RXCNTMSK;
}

uint16_t getrxfree()
{
	return ((rxrp - rxwp) & RXCNTMSK) - 1;
}

uint8_t buffwrite(const uint8_t * buff, uint8_t len)
{
	uint8_t i = 0;
	if (len)
		{
			while (i < len)
				{
					if (gettxfree())
						{
							puttxbuff(buff[i]);
							i++;
						}
				}
		}
	else
		{
			while (buff[i])
				{
					if (gettxfree())
						{
							puttxbuff(buff[i]);
							i++;
						}
				}
		}
	return i;
}

uint8_t buffread(uint8_t * buff, uint8_t len)
{
	uint8_t i = 0;
	while (i < len)
		{
			if (getrxfull())
				{
					buff[i] = getrxbuff();
					i++;
				}
			else
				break;
		}
	return i;
}


void flushrx()
{
	rxwp = rxrp;
}
