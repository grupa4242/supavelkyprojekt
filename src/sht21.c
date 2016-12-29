/*
 * sht21.c
 *
 *  Created on: 29. 12. 2016
 *      Author: Me
 */


#include "sht21.h"

#define TRIGTHOLD     0b11100011
#define TRIGRHHOLD    0b11100101
#define TRIGTNOHOLD   0b11110011
#define TRIGRHNOHOLD   0b00000101
#define USERREGW      0b11100110
#define USERREGR      0b11100111
#define SOFTRES       0b11111110
#define ADDRESS       0x80



Status SHT21_res()
{
	unsigned char data = SOFTRES;
	Status error = I2C_Master_BufferWriteWithoutRegisterAddress(&data, 1, ADDRESS);

	return error;
}

Status SHT21_blockMeasT(uint16_t *val)
{
	unsigned char data[3];
	Status error = I2C_Master_BufferRead(data, 3, ADDRESS, TRIGTHOLD);
	*val = (data[0] << 8) | (data[1] & 0xFC);
	return error;
}

Status SHT21_blockMeasRH(uint16_t *val)
{
	unsigned char data[3];
	Status error = I2C_Master_BufferRead(data, 3, ADDRESS,TRIGRHHOLD);
	*val = (data[0] << 8) | (data[1] & 0xFC);
	return error;
}

Status SHT21_trigMeasT()
{
	unsigned char data = TRIGTNOHOLD;
	Status error = I2C_Master_BufferWriteWithoutRegisterAddress(&data, 1, ADDRESS);
	return error;
}

Status SHT21_trigMeasRH()
{
	unsigned char data = TRIGRHNOHOLD;
	Status error = I2C_Master_BufferWriteWithoutRegisterAddress(&data, 1, ADDRESS);
	return error;
}

Status SHT21_readVal(uint16_t *val, uint8_t * status)
{
	unsigned char data[3];
	Status error = I2C_Master_BufferReadWithoutRegisterAddress(data, 1, ADDRESS);
	*val = (data[0] << 8) | (data[1] & 0xFC);
	if (error == AddressAckError)
		*status = 0;
	else if (error == Success)
	{
		if (data[1] & 0x02)
			*status = 1;
		else
			*status = 2;
	}

	return error;
}

inline Status SHT21_userRegWrite(uint8_t *val)
{
	unsigned char data;
	Status error = I2C_Master_BufferRead(val, 1, ADDRESS, USERREGR);
	data ^= data ^ (*val & 0xC7);
	Status error = I2C_Master_BufferWrite(val, 1, ADDRESS, USERREGW);
	return error;
}

inline Status SHT21_userRegRead(uint8_t *val)
{
	Status error = I2C_Master_BufferRead(val, 1, ADDRESS, USERREGR);
	return error;
}

