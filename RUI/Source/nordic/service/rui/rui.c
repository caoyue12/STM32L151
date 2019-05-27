#include <stdbool.h>
#include "nrf_assert.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "board_basic.h"
#include <stddef.h>
#include <string.h>
#include "app_error.h"
#include "nrf_soc.h"
#include "rui.h"
#include "gps.h"
#include "bme280.h"
#ifdef BG96_TEST
#include "bg96.h"
#endif
#ifdef BC95G_TEST
#include "bc95-g.h"
#endif
#ifdef M35_TEST
#include "m35.h"
#endif
#include "hal_uart.h"

double gps_lat = 0;
double gps_lon = 0;   
#if defined(LORA_81x_TEST) || defined(LORA_4600_TEST)
uint32_t lora_send(uint8_t *cmd);
#endif

extern GSM_RECEIVE_TYPE g_type;

#ifdef SHTC3_TEST

float g_humidity = 0;
uint32_t rui_temperature_get(double *temp)
{
    uint32_t ret = 1;
    float temp_t;
    if(temp == NULL)
    {
        return 1;
    }
    SHTC3_GetTempAndHumi(&temp_t,&g_humidity);

    *temp = temp_t;
    return 1;
}
uint32_t rui_humidity_get(double *humidity)
{
    if(humidity == NULL)
    {
        return 1;
    }
    *humidity = g_humidity;
    return 1;
}
#endif
#ifdef SHT31_TEST

float g_humidity = 0;
uint32_t rui_temperature_get(double *temp)
{
    uint32_t ret = 1;
    float temp_t;
    if(temp == NULL)
    {
        return 1;
    }

    if (Sht31_startMeasurementHighResolution() == 0)
      {
          Sht31_readMeasurement_ft(&g_humidity,&temp_t);
      }
     *temp = (double)temp_t;
     return ret;
}
uint32_t rui_humidity_get(double *humidity)
{
    if(humidity == NULL)
    {
        return 1;
    }
    *humidity = (double)g_humidity;
    return 1;
}
#endif

#ifdef LPS22HB_TEST
uint32_t rui_pressure_get(double *pressure)
{
    uint32_t ret = 1;
    float tmp = 0;
    if(pressure == NULL)
    {
        return 1;
    }
    
    ret = get_lps22hb_data(&tmp);
   *pressure = (double)tmp;
    
    return ret;
}
#endif

#ifdef BEM280_TEST
uint32_t rui_temperature_get(double *temp)
{
    uint32_t ret = 1;
    double tmp = 0;
    if(temp == NULL)
    {
        return 1;
    }
    ret = get_bme280_temp(&tmp);
    *temp = tmp;
    return ret;
}

uint32_t rui_humidity_get(double *humidity)
{
    uint32_t ret = 1;
    double tmp = 0;
    if(humidity == NULL)
    {
        return 1;
    }
    ret = get_bme280_humidity(&tmp);
    *humidity = tmp;
    return ret;
}

uint32_t rui_pressure_get(double *pressure)
{
    uint32_t ret = 1;
    double tmp = 0;
    if(pressure == NULL)
    {
        return 1;
    }
    ret = get_bme280_pressure(&tmp);
    *pressure = tmp;
    return ret;
}
#endif
#ifdef LIS3DH_TEST
uint32_t rui_acceleration_get(int *x, int *y, int *z)
{
    uint32_t ret = 0;
    if(x == NULL || y == NULL || z == NULL)
    {
        return 1;
    }
    ret = lis3dh_twi_init();
    if(ret != NRF_SUCCESS)
    {
        NRF_LOG_INFO( "lis3dh_twi_init fail %d\r\n", ret);
    }
    get_lis3dh_data(x,y,z);
    *x =*x * 4000/65536;
    *y =*y * 4000/65536;
    *z =*z * 4000/65536;    
    return ret;
}
#endif
#ifdef LIS2MDL_TEST
uint32_t rui_magnetic_get(float *magnetic_x, float *magnetic_y, float *magnetic_z)
{
    uint32_t ret = 0;
    if(magnetic_x == NULL || magnetic_y == NULL || magnetic_z == NULL)
    {
        return 1;
    }
    ret = lis2mdl_twi_init();
    if(ret != NRF_SUCCESS)
    {
        NRF_LOG_INFO( "lis2mdl_twi_init fail %d\r\n", ret);
    }
    get_lis2mdl_data(magnetic_x,magnetic_y,magnetic_z);
    return ret;
}
#endif
#ifdef OPT3001_TEST
uint32_t rui_light_strength_get(float *light_data)
{
    uint32_t ret = 0;
    if(light_data == NULL)
    {
        return 1;
    }
    ret = opt3001_twi_init();
    if(ret != NRF_SUCCESS)
    {
        NRF_LOG_INFO( "opt3001_twi_init fail %d\r\n", ret);
    }
    get_opt3001_data(light_data);

    return ret;
}
#endif
#ifdef BG96_TEST
uint32_t rui_gps_info_get(uint8_t *data, uint32_t len)
{
    uint32_t ret = 0;
    uint8_t i = 0;
    if(data == NULL || len < 0)
    {
        return 1;
    }
    gps_data_get(data,len);
    memcpy(data,&data[14],len-14);
    for (i = 0; data[i] !=0; i++)
    {
        
        if (data[i] == '\r' || data[i] == '\n')
        {

            break;
        }
    }
    memset(&data[i],0,127-i);
    //gps_parse(data);

    return ret;
}
extern char GSM_RSP[1600];
uint32_t rui_lte_response(uint8_t *rsp, uint32_t len, uint32_t timeout)
{
    if(rsp == NULL || len < 0)
    {
        return;
    }
    g_type = GSM_TYPE_CHAR;
    memset(GSM_RSP, 0, 1600);
    Gsm_WaitRspOK(GSM_RSP, timeout, true);
    NRF_LOG_INFO("%s\r\n",GSM_RSP);
    return 1;
}
#endif
#ifdef L70R_TEST
uint32_t rui_gps_info_get(uint8_t *data, uint32_t len)
{
    uint32_t ret = 0;
    if(data == NULL || len < 0)
    {
        return 1;
    }
    rak_uart_init(GPS_USE_UART,GPS_RXD_PIN,GPS_TXD_PIN,UART_BAUDRATE_BAUDRATE_Baud9600);
    delay_ms(2000);
    gps_data_get(data,len);
    delay_ms(800);
#ifdef M35_TEST
    rak_uart_init(GSM_USE_UART,GSM_RXD_PIN,GSM_TXD_PIN,UART_BAUDRATE_BAUDRATE_Baud115200);
#endif
#ifdef BC95G_TEST
    rak_uart_init(GSM_USE_UART,GSM_RXD_PIN,GSM_TXD_PIN,UART_BAUDRATE_BAUDRATE_Baud9600);
#endif
    return ret;
}
#endif
#if defined(BC95G_TEST) || defined(M35_TEST)
uint32_t rui_lte_response(uint8_t *rsp, uint32_t len, uint32_t timeout)
{
    if(rsp == NULL || len < 0)
    {
        return;
    }
    g_type = GSM_TYPE_CHAR;
    Gsm_WaitRspOK(rsp, timeout, true);
    return 1;
}
#endif
#if defined(BC95G_TEST) || defined(M35_TEST) || defined(BG96_TEST)
uint32_t rui_lte_send(uint8_t *cmd)
{
    Gsm_print(cmd);
    return 1;
}
#endif

#if defined(LORA_81x_TEST) || defined(LORA_4600_TEST)
uint32_t rui_lora_send(uint8_t *cmd)
{
    lora_send(cmd);
    return 1;
}
#endif

#ifdef MAX7_TEST
extern uint8_t GpsDataBuffer[512];
uint32_t rui_gps_info_get(uint8_t *data, uint32_t len)
{   
        uint8_t count = 8;
        if(data == NULL || len < 0)
        {
           return 1;
        }
        while(count--)
        {
            Max7GpsReadDataStream();
            if (GpsParseGpsData(GpsDataBuffer, 512))
            {
                GpsGetLatestGpsPositionDouble(&gps_lat, &gps_lon);
            }
            delay_ms(500);
        }
        sprintf(data,"gps: lat = %lf, lon = %lf",gps_lat,gps_lon);
        return 1;
}
#endif