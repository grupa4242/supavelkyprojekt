#ifndef __VRS_CV5_H_
#define __VRS_CV5_H_

#include <stdint.h>

void int_init();
void uart_init();
void gpio_init();
void adc_init();
void rtc_init();

uint32_t millis();

#endif //__VRS_CV5_H_
