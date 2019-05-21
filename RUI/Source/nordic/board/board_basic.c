#include "board_basic.h"
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "nordic_common.h"
#include "nrf.h"
#include "hal_uart.h"
#include "nrf_drv_rtc.h"
#include "nrf_rtc.h"

GSM_RECEIVE_TYPE g_type = GSM_TYPE_CHAR;

/**@brief Function for initializing the nrf log module.
 */
uint32_t get_rtc_counter(void)
{
    return NRF_RTC1->COUNTER;
} 

#if defined(LORA_81x_TEST) || defined(LORA_4600_TEST)

void TimerInit( TimerEvent_t *obj, void ( *callback )( void ) )
{
	if (obj == NULL)
		return;
	obj->id = &obj->timer;
	ret_code_t err_code = app_timer_create(&obj->id, APP_TIMER_MODE_SINGLE_SHOT, (app_timer_timeout_handler_t)callback);
	APP_ERROR_CHECK(err_code);
}

void TimerStart( TimerEvent_t *obj )
{
	if (obj->id == NULL || obj->timeout == 0)
		return;

	uint32_t ticks = APP_TIMER_TICKS(obj->timeout);
	if (ticks < APP_TIMER_MIN_TIMEOUT_TICKS)
		ticks = APP_TIMER_MIN_TIMEOUT_TICKS;

	ret_code_t err_code = app_timer_start(obj->id, ticks, NULL);
	APP_ERROR_CHECK(err_code);
}

void TimerStop( TimerEvent_t *obj )
{
	if (obj->id == NULL)
		return;

	ret_code_t err_code = app_timer_stop(obj->id);
	APP_ERROR_CHECK(err_code);
}

void TimerSetValue( TimerEvent_t *obj, uint32_t value )
{
	if (value < 10)
		value = 10;
	obj->timeout = value;
}

TimerTime_t TimerGetCurrentTime( void )
{
	return get_rtc_counter();
}

TimerTime_t TimerGetElapsedTime( TimerTime_t savedTime )
{
	uint32_t ts = get_rtc_counter();
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

#endif

void delay_ms(uint32_t ms)
{
    nrf_delay_ms(ms);
}


