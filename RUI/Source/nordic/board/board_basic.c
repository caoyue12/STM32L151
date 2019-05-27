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
const nrf_drv_rtc_t rtc = NRF_DRV_RTC_INSTANCE(2); 

/**@brief Function for initializing the nrf log module.
 */

uint32_t get_rtc_counter(void)
{
    //return NRF_RTC1->COUNTER;
    return nrf_drv_rtc_counter_get(&rtc);
} 


#define RTC_FREQUENCY 1000 /* Hz */
typedef void (*rtc_wakeup_callback_t)(void);
static uint32_t timestamp_base = 0;
static rtc_wakeup_callback_t wakeup_clbk = NULL;

static void rtc_handler(nrf_drv_rtc_int_type_t int_type)
{
	if (int_type == NRF_DRV_RTC_INT_OVERFLOW)
	{
		timestamp_base += nrf_drv_rtc_max_ticks_get(&rtc);
	}
	if (int_type == NRF_DRV_RTC_INT_COMPARE0)
	{
		if (wakeup_clbk)
			wakeup_clbk();
	}
}

uint32_t rtc_get_timestamp(void)
{
	uint32_t ticks = nrf_drv_rtc_counter_get(&rtc);
	NRF_LOG_INFO("ticks = %ld\r\n",timestamp_base  + ticks);
	return (timestamp_base  + ticks);
}

void rtc_update_timestamp(uint32_t timebase)
{
	timestamp_base = timebase;
}

void RtcInit(void)
{
	// Initialize RTC instance
	nrf_drv_rtc_config_t config = NRF_DRV_RTC_DEFAULT_CONFIG;
	config.prescaler = RTC_FREQ_TO_PRESCALER(RTC_FREQUENCY);
	ret_code_t err_code = nrf_drv_rtc_init(&rtc, &config, rtc_handler);
	APP_ERROR_CHECK(err_code);

	// Enable tick event - no interrupt, we need tick just to drive ADC
	nrf_drv_rtc_tick_enable(&rtc, false);
	nrf_drv_rtc_overflow_enable(&rtc, true);

	// Power on RTC instance
	nrf_drv_rtc_enable(&rtc);
}

void rtc_test_overflow(void)
{
	nrf_rtc_task_trigger(rtc.p_reg, NRF_RTC_TASK_TRIGGER_OVERFLOW);
}

ret_code_t rtc_schedule_wakeup(uint32_t timeout)
{
	uint32_t ticks = nrf_drv_rtc_counter_get(&rtc);
	uint32_t timeout_ticks = timeout*RTC_FREQUENCY;
	uint32_t compare_val = 0;
	uint32_t max_ticks = nrf_drv_rtc_max_ticks_get(&rtc);
	if (ticks + timeout_ticks > max_ticks)
		compare_val = (max_ticks - ticks) + timeout_ticks;
	else
		compare_val = ticks + timeout_ticks;

	return nrf_drv_rtc_cc_set(&rtc, 0, compare_val, true);
}
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
	return rtc_get_timestamp();
}

TimerTime_t TimerGetElapsedTime( TimerTime_t savedTime )
{
	uint32_t ts = rtc_get_timestamp();
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


void delay_ms(uint32_t ms)
{
    nrf_delay_ms(ms);
}


