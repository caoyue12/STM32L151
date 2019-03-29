#include "board_basic.h"
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "hal_uart.h"
#include "nrf_drv_rtc.h"
#include "nrf_rtc.h"

GSM_RECEIVE_TYPE g_type = GSM_TYPE_CHAR;
#ifdef LORA_TEST

const nrf_drv_rtc_t rtc = NRF_DRV_RTC_INSTANCE(2); /**< Declaring an instance of nrf_drv_rtc for RTC0. */

uint8_t BoardGetBatteryLevel( void )
{
    return 250; // full charged
}

uint32_t get_stamp(void)
{
    uint32_t ticks = xTaskGetTickCount();
    return ticks /portTICK_RATE_MS;
}

#endif

void delay_ms(uint32_t ms)
{
    nrf_delay_ms(ms);
}


void TimerInit( TimerEvent_t *obj, void ( *callback )( void ) )
{
	obj->id = xTimerCreate(" ", 1000,pdFALSE,NULL, (TimerCallbackFunction_t) callback);
       if(obj->id == NULL)
       {
           NRF_LOG_INFO("xTimerCreate fail\r\n");
       }
	obj->timeout = 1000;
}

void TimerStart( TimerEvent_t *obj )
{
	if (obj->id == NULL || obj->timeout == 0)
		return;

	uint32_t ticks = APP_TIMER_TICKS(obj->timeout);
	if (ticks < APP_TIMER_MIN_TIMEOUT_TICKS)
		ticks = APP_TIMER_MIN_TIMEOUT_TICKS;
        if(xTimerStart(obj->id, obj->timeout) != pdPASS)
        {
                NRF_LOG_INFO("TimerStart fail\r\n");
        }

}

void TimerStop( TimerEvent_t *obj )
{
	if (obj->id == NULL)
		return;
       if(xTimerStop(obj->id, obj->timeout) != pdPASS)
       {
            NRF_LOG_INFO("TimerStop fail\r\n");
       }
}

void TimerSetValue( TimerEvent_t *obj, uint32_t value )
{
	if (value < 10)
		value = 10;
       if(xTimerChangePeriod(obj->id, value /portTICK_RATE_MS, 100 )!= pdPASS)
       {
            NRF_LOG_INFO("xTimerChangePeriod fail\r\n");
       }
}

TimerTime_t TimerGetCurrentTime( void )
{
	return get_stamp();
}

TimerTime_t TimerGetElapsedTime( TimerTime_t savedTime )
{
	uint32_t ts = get_stamp();
	TimerTime_t elapsed = (TimerTime_t)0;
	if (savedTime < ts)
		elapsed = (TimerTime_t)(ts - savedTime);

	return elapsed;
}

void TimerLowPowerHandler( void )
{
}

TimerTime_t TimerTempCompensation( TimerTime_t period, float temperature )
{
	return period;
}

