/*
 * ads1100.c
 *
 *  Created on: 24. 12. 2016
 *      Author: Me
 */

#include "ads1100.h"

#define ADS1100_ADDRESS(a) (0x90 | ((a << 1) & 0xE))

#define STBSY (1 << 7)

#define SC (1 << 4)

#define DR8RES16 (3<<2)
#define DR16RES15 (2<<2)
#define DR32RES14 (1<<2)
#define DR128RES12 (0<<2)

#define PGA1 0
#define PGA2 1
#define PGA4 2
#define PGA8 3

#define CONFIGMASK (PGA8 | DR8RES16)

#define CONFIG(x) ((x) & CONFIGMASK)

#define CFGREG CONFIG(DR8RES16 | PGA1)

Status ADS1100_ContConvInit()
{
	unsigned char data = CFGREG;
	Status error = I2C_Master_BufferWriteWithoutRegisterAddress(&data, 1, ADS1100_ADDRESS(0));

	return error;
}

Status ADS1100_StartConv()
{
	unsigned char data = CFGREG | STBSY | SC;
	Status error = I2C_Master_BufferWriteWithoutRegisterAddress(&data, 1, ADS1100_ADDRESS(0));

	return error;
}

Status ADS1100_readData(int16_t *data)
{
	unsigned char buffer[2];

	Status error = I2C_Master_BufferReadWithoutRegisterAddress(buffer, 2, ADS1100_ADDRESS(0));

	*data = (buffer[0]<<8) + buffer[1];

	return error;
}

Status ADS1100_readDataStatus(int16_t * data, uint8_t * status)
{
	unsigned char buffer[3];

	Status error = I2C_Master_BufferReadWithoutRegisterAddress(buffer, 3, ADS1100_ADDRESS(0));

	*data = (buffer[0]<<8) + buffer[1];
	*status = !(buffer[2] & STBSY);

	return error;
}
