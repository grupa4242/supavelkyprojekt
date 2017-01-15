#include "core.h"
#include <stddef.h>
#include "stm32l1xx.h"
#include "coreit.h"

volatile uint32_t SysUpTime = 0;

uint32_t millis()
{
	return SysUpTime;
}

void gpio_init()
{
	GPIO_InitTypeDef conf;

	RCC_AHBPeriphClockCmd (RCC_AHBPeriph_GPIOA, ENABLE);

	conf.GPIO_Speed = GPIO_Speed_40MHz;
	conf.GPIO_PuPd = GPIO_PuPd_NOPULL;
	conf.GPIO_OType = GPIO_OType_PP;

	conf.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	conf.GPIO_Mode = GPIO_Mode_AF;

	GPIO_Init (GPIOA, &conf);

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_USART1);

}

void uart_init()
{
	USART_InitTypeDef conf;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);

	conf.USART_BaudRate = 9600;
	conf.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	conf.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	conf.USART_Parity = USART_Parity_No;
	conf.USART_StopBits = USART_StopBits_1;
	conf.USART_WordLength = USART_WordLength_8b;

	USART_Init(USART1, &conf);

	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

	USART_Cmd(USART1, ENABLE);
}

void rtc_init()
{
#define LSE_STARTUP_TIMEOUT (0x0200)

	__IO uint32_t StartUpCounter = 0, LSEStatus = 0;
	RTC_InitTypeDef rtc_conf;
	RTC_StructInit(&rtc_conf);

	RCC_APB1PeriphClockCmd(RCC_APB1ENR_PWREN, ENABLE);
	PWR_RTCAccessCmd(ENABLE);

	if (!(RCC->CSR & RCC_CSR_RTCEN))
		{
			RCC_LSEConfig(RCC_LSE_ON);
			do
			  {
			    LSEStatus = RCC->CSR & RCC_CSR_LSERDY;
			    StartUpCounter++;
			  } while((LSEStatus == 0) && (StartUpCounter != LSE_STARTUP_TIMEOUT));

			if (LSEStatus)
				{
					RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
				}
			else if(RCC->CR & RCC_CR_HSERDY)
				{
					RCC_RTCCLKConfig(RCC_RTCCLKSource_HSE_Div16);
					rtc_conf.RTC_SynchPrediv = 3905;
				}
			else
				{
					RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
					rtc_conf.RTC_SynchPrediv = 288;
				}

			RCC_RTCCLKCmd(ENABLE);
			RTC_Init(&rtc_conf);
		}

	RTC_WakeUpCmd(DISABLE);
	RTC_WakeUpClockConfig(RTC_WakeUpClock_CK_SPRE_16bits);
	RTC_SetWakeUpCounter(5*60);
	RTC_WakeUpCmd(ENABLE);

	if (RTC_GetFlagStatus(RTC_FLAG_INITS))
		{
			//TODO: set time/date
			RTC_TimeTypeDef time;
			RTC_DateTypeDef date;
			RTC_TimeStructInit(&time);
			RTC_DateStructInit(&date);
			RTC_SetTime(RTC_Format_BCD, &time);
			RTC_SetDate(RTC_Format_BCD, &date);
		}

	PWR_RTCAccessCmd(DISABLE);
}

void int_init()
{
	NVIC_InitTypeDef conf;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	conf.NVIC_IRQChannelCmd = ENABLE;
	conf.NVIC_IRQChannelPreemptionPriority = 1;
	conf.NVIC_IRQChannelSubPriority = 0;
	conf.NVIC_IRQChannel = USART1_IRQn;

	NVIC_Init(&conf);
}
