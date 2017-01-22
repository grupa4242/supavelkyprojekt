#ifndef __VRS_CV5_H_
#define __VRS_CV5_H_

#include <stdint.h>
#include "stm32l1xx.h"

void int_init();
void uart_init();
void gpio_init();
void adc_init();
void rtc_init();

uint32_t millis();

void rtc_settimedate(RTC_TimeTypeDef * time, RTC_DateTypeDef * date);

#define COLLECTPERIOD (5*60 - 1) //[s]

#endif //__VRS_CV5_H_
