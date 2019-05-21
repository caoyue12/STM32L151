#include <stdbool.h>
#include "nrf_assert.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "board_basic.h"
#include <stddef.h>
#include <string.h>
#include "app_error.h"
#include "nrf_soc.h"
#include "sensor.h"

#ifdef BG96_TEST
#include "bg96.h"
#endif
#ifdef BC95G_TEST
#include "bc95-g.h"
#endif
#ifdef M35_TEST
#include "m35.h"
#endif

#ifdef L70R_TEST
extern void Gps_standby(void);
#endif



#if defined(LORA_81x_TEST) || defined(LORA_4600_TEST)
extern void SX1276SetSleep( void );
#endif

int POWER_SAVE_ON  =  0;
void power_save_open()
{
    NRF_LOG_INFO("power save open!\r\n");
    POWER_SAVE_ON = 1;

#if defined(BC95G_TEST) || defined(M35_TEST) || defined(BG96_TEST)
    Gsm_PowerDown();
#endif

#ifdef L70R_TEST
    Gps_standby();
#endif

#ifdef MAX7_TEST
    gps_poweroff();
#endif
#ifdef BEM280_TEST
    bme280_spi_init();
    _bme280_sleep_init();
#endif

#ifdef OPT3001_TEST
    opt3001_twi_init();
    sensorOpt3001Enable(0);
#endif

#ifdef LIS2MDL_TEST
    lis2mdl_twi_init();
    lis2mdl_sleep_init();
#endif
#ifdef LIS3DH_TEST  
    lis3dh_twi_init();
    lis3dh_sleep_init();
#endif
#ifdef SHTC3_TEST
    SHTC3_Init();
    SHTC3_Sleep();
#endif

#if defined(LORA_81x_TEST) || defined(LORA_4600_TEST)
   	SX1276SetSleep( );
#endif
	app_uart_close();
    *(volatile uint32_t *)0x40002FFC = 0;
}
void power_save_close()
{
	if(POWER_SAVE_ON == 1)
	{
	 *(volatile uint32_t *)0x40002FFC = 1;
    	NRF_LOG_INFO("power save close!\r\n");
#if defined(BC95G_TEST) || defined(M35_TEST) || defined(BG96_TEST)
    	Gsm_Init();
#endif
#ifdef L70R_TEST
       Gps_Init();
#endif

#ifdef MAX7_TEST
       max_init();
       gps_setup();
#endif
       sensors_init();
       POWER_SAVE_ON = 0;
    }
}

#ifdef  BATTERY_LEVEL_SUPPORT

void battery_level(void)
{
    nrf_saadc_value_t saadc_val;
    float voltage = 0;
    int adc_num = 3;
    nrf_saadc_value_t adc_avg = 0;
    nrf_saadc_value_t adc_sum = 0;
    for (int i = 0; i < adc_num; ++i)
    {
        nrf_drv_saadc_sample_convert(0,&saadc_val);
        adc_sum+=saadc_val;
        delay_ms(300);
    }
    adc_avg = adc_sum/adc_num;
    voltage = ((float)adc_avg * 3.6 / 1024) * 5 / 3;
    //NRF_LOG_INFO("voltage = %lf V\r\n",voltage);
    NRF_LOG_INFO("Battery Voltage = "NRF_LOG_FLOAT_MARKER" V !\r\n", NRF_LOG_FLOAT(voltage));
}



void saadc_callback(nrf_drv_saadc_evt_t const *p_event){}


void saadc_init(void)
{
    ret_code_t err_code;

    nrf_saadc_channel_config_t mmysaadc= NRF_DRV_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN1);

    err_code=nrf_drv_saadc_init(NULL,saadc_callback);
    APP_ERROR_CHECK(err_code);
    err_code = nrf_drv_saadc_channel_init(0, &mmysaadc);
    APP_ERROR_CHECK(err_code); 
}

#endif