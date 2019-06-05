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
#include "radio.h"
extern Uart_t Uart1;
TimerEvent_t time1;
TimerEvent_t time2;
void time1call(void)
{	
	printf("time1call\r\n");

}
void time2call(void)
{
	printf("time2call\r\n");
}

static RadioEvents_t RadioEvents;

void OnTxDone( void );

/*!
 * \brief Function to be executed on Radio Rx Done event
 */
void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr );

/*!
 * \brief Function executed on Radio Tx Timeout event
 */
void OnTxTimeout( void );

/*!
 * \brief Function executed on Radio Rx Timeout event
 */
void OnRxTimeout( void );

/*!
 * \brief Function executed on Radio Rx Error event
 */
void OnRxError( void );

int main( void )
{
   uint8_t Buffer[4];
   Buffer[0] = 'P';
   Buffer[1] = 'O';
   Buffer[2] = 'N';
   Buffer[3] = 'G';
//   LoRaMacPrimitives_t LoRaMacPrimitives;
//   LoRaMacCallback_t LoRaMacCallbacks;
//   MibRequestConfirm_t mibReq;
	unsigned char datetemp;
	BoardInitMcu( );
//  BoardInitPeriph( );
	TimerInit(&time1,time1call);
//	TimerInit(&time2,time2call);
//	TimerSetValue(&time1,2000);
//	TimerSetValue(&time2,1000);
	   
//	TimerStart(&time1 );
	
	
	

    RadioEvents.TxDone = OnTxDone;
    RadioEvents.RxDone = OnRxDone;
    RadioEvents.TxTimeout = OnTxTimeout;
    RadioEvents.RxTimeout = OnRxTimeout;
    RadioEvents.RxError = OnRxError;

    Radio.Init( &RadioEvents );
	
	
	


    Radio.SetChannel( 470000000 );
	
	Radio.SetTxConfig( MODEM_LORA, 14, 0, 0,
                                   7, 1,
                                   8, 1,
                                   true, 0, 0, 1, 3000 );
	
	Radio.SetRxConfig( MODEM_LORA, 0, 7,
                                   1, 0, 8,
                                   5, false,
                                   0, true, 0, 0, false, true );
	//Radio.Rx( 1000 );
	Radio.Send( Buffer, 4 );
    while( 1 )
    {
//	printf("\r\nSX1276Read(0X42)	%02X\r\n",SX1276Read(0X42));
	//printf("\r\nSX1276Read(0X02)	%02X\r\n",SX1276Read(0X02));
	DelayMs(5000);
	
	  
		
    }
}



void OnTxDone( void )
{
    Radio.Sleep( );
//    State = TX;
printf("OnTxDone\r\n");
}

void OnRxDone( uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr )
{
    Radio.Sleep( );
//    BufferSize = size;
//    memcpy( Buffer, payload, BufferSize );
//    RssiValue = rssi;
//    SnrValue = snr;
//    State = RX;
}

void OnTxTimeout( void )
{
    Radio.Sleep( );
//    State = TX_TIMEOUT;
printf("OnTxTimeout\r\n");
}

void OnRxTimeout( void )
{
    Radio.Sleep( );
	printf("OnRxTimeout\r\n");
//    State = RX_TIMEOUT;
}

void OnRxError( void )
{
    Radio.Sleep( );
//    State = RX_ERROR;
}

