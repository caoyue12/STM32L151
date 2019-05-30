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
#include "LoRaMac.h"
#include "Commissioning.h"
#include "stdio.h"
#include "delay.h"
#include "uart.h"
extern Uart_t Uart1;


int main( void )
{
//   LoRaMacPrimitives_t LoRaMacPrimitives;
//   LoRaMacCallback_t LoRaMacCallbacks;
//   MibRequestConfirm_t mibReq;

	BoardInitMcu( );
//   BoardInitPeriph( );

//   DeviceState = DEVICE_STATE_INIT;

    while( 1 )
    {
		UartPutChar(&Uart1,0x88);
		
		DelayMs(1000);
		
    }
}
