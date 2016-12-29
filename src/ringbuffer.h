/*
 * ringbuffer.h
 *
 *  Created on: 25. 10. 2016
 *      Author: Lenka
 */

#ifndef RINGBUFFER_H_
#define RINGBUFFER_H_

#define TXBUFFLEN (1 << 7) //64
#define RXBUFFLEN (1 << 7)
#include <stdint.h>

char gettxbuff();
void puttxbuff(char c);

uint16_t gettxfull();
uint16_t gettxfree();



#endif /* RINGBUFFER_H_ */
