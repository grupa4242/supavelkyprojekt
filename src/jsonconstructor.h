/*
 * jsonconstructor.h
 *
 *  Created on: 15. 1. 2017
 *      Author: Me
 */

#ifndef JSONCONSTRUCTOR_H_
#define JSONCONSTRUCTOR_H_

#include "datastore.h"

void constructhead(char * str, uint32_t stationid);
void constructentry(char * str, datasample * entry, uint8_t notlast);
void constructtail(char * str);

#endif /* JSONCONSTRUCTOR_H_ */
