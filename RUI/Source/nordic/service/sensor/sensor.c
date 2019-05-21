#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "nordic_common.h"
#include "nrf.h"
#include "nrf_sdh.h"
#include "app_timer.h"
#include "fds.h"
#include "app_uart.h"
#include "app_util_platform.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "board_basic.h"
#include "nrf_power.h"
#include "hal_uart.h"


void sensors_init()
{
    int ret;
#if defined(BC95G_TEST) || defined(M35_TEST) || defined(BG96_TEST)
    // init gsm
    Gsm_Init();
#endif
#ifdef L70R_TEST
    // init gps
    Gps_Init();
#endif

#if defined(LORA_81x_TEST) || defined(LORA_4600_TEST)
    lora_init();
#endif

#ifdef BEM280_TEST
    ret = bme280_spi_init();
    if(ret != NRF_SUCCESS)
    {
        NRF_LOG_INFO( "bme280_spi_init fail %d\r\n", ret);
    }
    else
    {
        ret = _bme280_init();
        if(ret < 0)
        {
            NRF_LOG_INFO( "lis3dh_init fail\r\n");
        }
    }
#endif
#ifdef LIS3DH_TEST
    //config interrupt   
    ret = lis3dh_twi_init();
    if(ret != NRF_SUCCESS)
    {
        NRF_LOG_INFO( "lis3dh_twi_init fail %d\r\n", ret);
    }
    else
    {
        ret = lis3dh_init();
        if(ret < 0)
        {
            NRF_LOG_INFO( "lis3dh_init fail\r\n");
        }
    }
#endif
#ifdef LIS2MDL_TEST
    ret = lis2mdl_twi_init();
    if(ret < 0)
    {
        NRF_LOG_INFO( "lis2mdl_twi_init fail %d\r\n", ret);
    }
    else
    {
        ret = lis2mdl_init();
        if(ret < 0)
        {
            NRF_LOG_INFO( "lis2mdl_init fail\r\n");
        }
    }
#endif
#ifdef OPT3001_TEST
    ret = opt3001_twi_init();
    if(ret < 0)
    {
        NRF_LOG_INFO( "opt3001_twi_init fail %d\r\n", ret);
    }
    else
    {
        ret = opt3001_init();
        if(ret < 0)
        {
            NRF_LOG_INFO( "opt3001_init fail\r\n");
        }
    }
#endif

#ifdef SHT31_TEST
    ret = sht31_init();
    if(ret < 0)
    {
        NRF_LOG_INFO( "sht31_init fail %d\r\n", ret);
    }
#endif

#ifdef MAX7_TEST
    ret =max_init();
    if(ret < 0)
    {
        NRF_LOG_INFO( "max_init fail %d\r\n", ret);
    }
    gps_setup();
#endif

#ifdef SHTC3_TEST
    SHTC3_Init();
#endif

#ifdef LPS22HB_TEST

       ret = lps22hb_twi_init();
       if(ret < 0)
       {
        	NRF_LOG_INFO( "lps22hb_twi_init fail %d\r\n", ret);
       }
       	lps22hb_init();
#endif

#ifdef  BATTERY_LEVEL_SUPPORT
        saadc_init();
#endif
}
