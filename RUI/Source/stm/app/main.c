/*!
* \file      main.c
*
* \brief     LoRaMac classA device implementation
*
* \copyright Revised BSD License, see section \ref LICENSE.
*
* \code
*                ______                              _
*               / _____)             _              | |
*              ( (____  _____ ____ _| |_ _____  ____| |__
				  *               \____ \| ___ |    (_   _) ___ |/ ___)  _ \
				 *               _____) ) ____| | | || |_| ____( (___| | | |
						 *              (______/|_____)_|_|_| \__)_____)\____)_| |_|
*              (C)2013-2017 Semtech
*
* \endcode
*
* \author    Miguel Luis ( Semtech )
*
* \author    Gregory Cristian ( Semtech )
*/

/*! \file classA/NucleoL073/main.c */

#include "utilities.h"
#include "board.h"
#include "gpio.h"
//#include "LoRaMac.h"
//#include "Commissioning.h"
#include "stdio.h"
#include "delay.h"
#include "uart.h"
#include "timer.h"

extern Uart_t Uart1;
TimerEvent_t time1;
TimerEvent_t time2;
void time1call(void)
{	TimerStart(&time1 );
	printf("time1call\r\n");
}
void time2call(void)
{
	printf("time2call\r\n");
}



int main( void )
{
//   LoRaMacPrimitives_t LoRaMacPrimitives;
//   LoRaMacCallback_t LoRaMacCallbacks;
//   MibRequestConfirm_t mibReq;
	unsigned char datetemp;
	BoardInitMcu( );
//  BoardInitPeriph( );
	TimerInit(&time1,time1call);
//	   TimerInit(&time2,time2call);
	TimerSetValue(&time1,2000);
//	   TimerSetValue(&time2,1000);
	   
	TimerStart(&time1 );
//     TimerStart(&time2 );
//   DeviceState = DEVICE_STATE_INIT;
	SX1276Reset();
    while( 1 )
    {
	printf("\r\nSX1276Read(0X42)	%02X\r\n",SX1276Read(0X42));
	printf("\r\nSX1276Read(0X02)	%02X\r\n",SX1276Read(0X02));
	DelayMs(5000);
	printf("\r\nhello\r\n");	
	  
		
    }
}
