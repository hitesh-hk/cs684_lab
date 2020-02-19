#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "inc/tm4c123gh6pm.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/systick.h"
#include "driverlib/adc.h"
#include "driverlib/rom.h"
#include "driverlib/debug.h"
#include <time.h>
#include <inc/hw_gpio.h>
#include "driverlib/ssi.h"
#include "driverlib/uart.h"


uint32_t left_right_val[4];
uint32_t up_down_val[4];

volatile uint32_t position_avg[2];
volatile uint32_t temp[2];
volatile uint8_t digit[8];


void uart_char(char data)
{
	UARTCharPut(UART0_BASE, data);
}

int main(void) {
	
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);//This is isabling the speaker
	GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4);
	GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0x10);

	 
	 SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	 SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);
	
	 ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
	 ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_CH6);
	 ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_CH6);
	 ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_CH6);
	 ADCSequenceStepConfigure(ADC0_BASE,1,3,ADC_CTL_CH6|ADC_CTL_IE|ADC_CTL_END);
	 ADCSequenceEnable(ADC0_BASE, 1);

	 ADCSequenceConfigure(ADC1_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
	 ADCSequenceStepConfigure(ADC1_BASE, 1, 0, ADC_CTL_CH7);
	 ADCSequenceStepConfigure(ADC1_BASE, 1, 1, ADC_CTL_CH7);
	 ADCSequenceStepConfigure(ADC1_BASE, 1, 2, ADC_CTL_CH7);
	 ADCSequenceStepConfigure(ADC1_BASE,1,3,ADC_CTL_CH7|ADC_CTL_IE|ADC_CTL_END);
	 ADCSequenceEnable(ADC1_BASE, 1);

	SysCtlClockSet(SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 9600,(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));

	while (1)
	{
		
		 ADCIntClear(ADC0_BASE, 1);
		 ADCProcessorTrigger(ADC0_BASE, 1);
		 ADCIntClear(ADC1_BASE, 1);
		 ADCProcessorTrigger(ADC1_BASE, 1);
		while(!ADCIntStatus(ADC0_BASE, 1, false))
		{
		}
		while(!ADCIntStatus(ADC1_BASE, 1, false))
		{
		}
		 ADCSequenceDataGet(ADC0_BASE, 1, left_right_val);
		 ADCSequenceDataGet(ADC1_BASE, 1, up_down_val);

		position_avg[0] = (left_right_val[0] + left_right_val[1] + left_right_val[2] + left_right_val[3] + 2)/4;
		temp[0]=position_avg[0];
		position_avg[1] = (up_down_val[0] + up_down_val[1] + up_down_val[2] + up_down_val[3] + 2)/4;
		temp[1]=position_avg[1];

		digit[0]=temp[0]%10+48;
		temp[0]=temp[0]/10;
		digit[1]=temp[0]%10+48;
		temp[0]=temp[0]/10;
		digit[2]=temp[0]%10+48;
		temp[0]=temp[0]/10;
		digit[3]=temp[0]%10+48;
		temp[0]=temp[0]/10;
		digit[4]=temp[1]%10+48;
		temp[1]=temp[1]/10;
		digit[5]=temp[1]%10+48;
		temp[1]=temp[1]/10;
		digit[6]=temp[1]%10+48;
		temp[1]=temp[1]/10;
		digit[7]=temp[1]%10+48;
		temp[1]=temp[1]/10+48;

		uart_char(digit[3]);
		uart_char(digit[2]);
		uart_char(digit[1]);
		uart_char(digit[0]);		
		uart_char(',');
		uart_char(digit[7]);
		uart_char(digit[6]);
		uart_char(digit[5]);
		uart_char(digit[4]);		
		uart_char('\n');
		
		SysCtlDelay(670000);
	}
}
