/*
 * ads1100.h
 *
 *  Created on: 24. 12. 2016
 *      Author: Me
 */

#ifndef __ADS1100_H
#define __ADS1100_H

#include "i2c.h"
#include <stdint.h>

Status ADS1100_ContConvInit();
Status ADS1100_StartConv();
Status ADS1100_readData(uint16_t *data);
Status ADS1100_readDataStatus(uint16_t * data, uint8_t * status);

#endif
