/*
 * sht21.h
 *
 *  Created on: 29. 12. 2016
 *      Author: Me
 */

#ifndef SHT21_H_
#define SHT21_H_

#include "i2c.h"
#include <stdint.h>

Status SHT21_res();
Status SHT21_blockMeasT(uint16_t *val);
Status SHT21_blockMeasRH(uint16_t *val);
Status SHT21_trigMeasT();
Status SHT21_trigMeasRH();
Status SHT21_readVal(uint16_t *val, uint8_t * status);
Status SHT21_userRegWrite(uint8_t *val);
Status SHT21_userRegRead(uint8_t *val);

#endif /* SHT21_H_ */
