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
#define TRIGRHNOHOLD  0b11110101
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

Status SHT21_userRegWrite(uint8_t *val)
{
	unsigned char data;
	Status error = I2C_Master_BufferRead(&data, 1, ADDRESS, USERREGR);
	data ^= data ^ (*val & 0xC7);
	error = I2C_Master_BufferWrite(&data, 1, ADDRESS, USERREGW);
	return error;
}

Status SHT21_userRegRead(uint8_t *val)
{
	Status error = I2C_Master_BufferRead(val, 1, ADDRESS, USERREGR);
	return error;
}

float SHT21_CalcRH(uint16_t u16sRH)
//==============================================================================
{
	float humidityRH; // variable for result
	u16sRH &= ~0x0003; // clear bits [1..0] (status bits)
	//-- calculate relative humidity [%RH] --
	humidityRH = -6.0 + 125.0/65536 * (float)u16sRH; // RH= -6 + 125 * SRH/2^16
	return humidityRH;
}

float SHT21_CalcTemperatureC(uint16_t u16sT)
//==============================================================================
{
	float temperatureC; // variable for result
	u16sT &= ~0x0003; // clear bits [1..0] (status bits)
	//-- calculate temperature [°C] --
	temperatureC= -46.85 + 175.72/65536 *(float)u16sT; //T= -46.85 + 175.72 * ST/2^16
	return temperatureC;
}

uint8_t SHT21_CheckCrc(uint8_t data[], uint8_t nbrOfBytes, uint8_t checksum)
//==============================================================================
{
	uint8_t crc = 0;
	uint8_t byteCtr;
	//calculates 8-Bit checksum with given polynomial
	for (byteCtr = 0; byteCtr < nbrOfBytes; ++byteCtr)
		{ crc ^= (data[byteCtr]);
		for (uint8_t bit = 8; bit > 0; --bit)
			{ if (crc & 0x80) crc = (crc << 1) ^ 0x131;
			else crc = (crc << 1);
			}
		}
	if (crc != checksum) return 1;
	else return 0;
}
