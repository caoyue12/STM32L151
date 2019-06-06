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
uint8_t Buffer[6];
int main( void )
{
   //uint8_t Buffer[4];
   Buffer[0] = 'P';
   Buffer[1] = 'O';
   Buffer[2] = 'N';
   Buffer[3] = 'G';
   Buffer[2] = '\r';
   Buffer[3] = '\n';
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
//	
//	
//	


    Radio.SetChannel( 470000000 );
//	
	Radio.SetTxConfig( MODEM_LORA, 12, 0, 0,
                                   7, 1,
                                   8, 1,
                                   true, 0, 0, 1, 5000 );
	
	Radio.SetRxConfig( MODEM_LORA, 0, 7,
                                   1, 0, 8,
                                   5, false,
                                   0, true, 0, 0, false, true );
	Radio.Rx(0);
	//Radio.Send( Buffer, 4 );
    while( 1 )
    {
//	    Radio.Send( Buffer, 4 );
//		SX1276Write(0x11,0xf7);
        //printf("SX1276Read(0x11)	%02X\r\n",SX1276Read(0x11));
		//printf("SX1276Read(0x12)	%02X\r\n",SX1276Read(0x12));
//		printf("SX1276Read(0x12)	%02X\r\n",SX1276Read(0x12));
//		printf("SX1276Read(0x42)	%02X\r\n",SX1276Read(0x42));
//		printf("SX1276Read(0x44)	%02X\r\n",SX1276Read(0x44));
		
//		while(!(0x08&(SX1276Read(0x12))));
//		printf("Reg TXdone OK\r\n");
		Delay(2);
		
		
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
//    unsigned char* Buffer;
    memcpy( Buffer, payload, 4 );
	
//    RssiValue = rssi;
//    SnrValue = snr;
//    State = RX;
    
//   printf("OnRxDone %02X  %02X  %02X  %02X\r\n",Buffer[0],Buffer[1],Buffer[2],Buffer[3]);
	printf("OnRxDone %s\r\n",Buffer);
	Radio.Rx(0);
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

