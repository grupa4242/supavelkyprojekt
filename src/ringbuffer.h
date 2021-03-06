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

char getrxbuff();
void putrxbuff(const char c);

uint16_t getrxfull();
uint16_t getrxfree();

uint8_t buffwrite(const uint8_t * buff, uint8_t len);
uint8_t buffread(uint8_t * buff, uint8_t len);

void flushrx();

#endif /* RINGBUFFER_H_ */
