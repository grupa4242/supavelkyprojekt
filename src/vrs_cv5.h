#ifndef __VRS_CV5_H_
#define __VRS_CV5_H_

extern uint16_t adc_conv_val;
extern volatile uint16_t printmode;


void int_init();
void uart_init();
void gpio_init();
void adc_init();

#endif //__VRS_CV5_H_
