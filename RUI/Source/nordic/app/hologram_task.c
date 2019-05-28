#include "board_basic.h"
#include "boards.h"
#include "nrf_delay.h"
#include "nrf_drv_rtc.h"
#include "nrf_rtc.h"
#include <string.h>
#include "sensor.h"
#include "rui.h"
#include "nrf_log.h"


#if defined(BC95G_TEST) || defined(M35_TEST) || defined(BG96_TEST)
extern uint8_t cmd[128];
#endif

extern double gps_lat;
extern double gps_lon;  


void hologram_cmd_packet(uint8_t *key, uint8_t *data)
{
    uint8_t i = 0;
    uint8_t j = 0;
    cmd[0]= '{';
    cmd[1]= '\"';
    cmd[2]= 'k';
    cmd[3]= '\"';
    cmd[4]= ':';  
    cmd[5]= '\"';
    for (i = 0; i < 8; i++)
    {
        cmd[6+i] = key[j++];
    }
    cmd[14] = '\"';
    cmd[15] = ',';
    cmd[16]= '\"';
    cmd[17]= 'd';
    cmd[18]= '\"';
    cmd[19]= ':';  
    cmd[20]= '\"';
    j = 0;
    for (i = 0; i < 256; i++)
    {
        if (data[j] != 0)
        {
            cmd[21+i] = data[j++];
        }
        else
        {
            break;
        }
    }    
    cmd[21+j]='\"';
    cmd[22+j]=',';
    cmd[23+j]='\"'; 
    cmd[24+j]='t';       
    cmd[25+j]='\"';
    cmd[26+j]=':';
    cmd[27+j]='\"';  
    cmd[28+j]='T'; 
    cmd[29+j]='O'; 
    cmd[30+j]='P'; 
    cmd[31+j]='I'; 
    cmd[32+j]='C';
    cmd[33+j]='1'; 
    cmd[34+j]='\"';
    cmd[35+j]='}';                
}
void nb_iot_task(void)
{
    uint8_t rsp[500] = {0};
    uint8_t device_key[9] = {0};
    uint8_t test_data[256] = {0};
	uint8_t gps_data[128] = {0};
    uint8_t len[20] = {0}; 
    uint8_t sensor_len = 0;
    double temp = 0;
    double humidity = 0;
    double pressure = 0;
    int x = 0;
    int y = 0;
    int z = 0;
    float magnetic_x = 0;
    float magnetic_y = 0;
    float magnetic_z = 0;
    float light = 0;
    double lat = 0;
    double lon = 0;
    uint8_t i =0;
    uint8_t j =0;

    if(strstr(cmd,"SEND")!= NULL)
    {
        for (i = 5; i < 13; i++)
        {
            device_key[j++] = cmd[i];
        }
        j = 0;
        for (i = 14; i < 256; i++)
        {
            if (cmd[i] != 0)
            {
                test_data[j++] = cmd[i];
            }
            else
            {
                break;
            }
        }
        memset(cmd,0,128);
        hologram_cmd_packet(device_key,test_data);
        NRF_LOG_INFO("device_key = %s\r\n",device_key);
        NRF_LOG_INFO("test_data = %s\r\n",test_data); 
        NRF_LOG_INFO("send packet = %s\r\n",cmd);  
        rui_lte_send("AT+QIOPEN=1,0,\"TCP\",\"cloudsocket.hologram.io\",9999,0,1");
        memset(rsp, 0, 500);
        rui_lte_response(rsp, 500, 500 * 60);
        memset(rsp, 0, 500);
        rui_lte_response(rsp, 500, 500 * 20);
        delay_ms(500);
        memset(len,0,20);
        sprintf(len,"AT+QISEND=0,%d",36+j+1);
        rui_lte_send(len);
        delay_ms(500);                              
        rui_lte_send(cmd);
        memset(rsp, 0, 500);
        rui_lte_response(rsp, 500, 500 * 60);
        memset(rsp, 0, 500);
        rui_lte_response(rsp, 500, 500 * 80);
        rui_lte_send("AT+QICLOSE=0,30000");
        memset(rsp, 0, 500);
        rui_lte_response(rsp, 500, 500 * 60);
        }
        else if (strstr(cmd,"SENSOR")!= NULL)
        {
            for (i = 7; i < 15; i++)
            {
                device_key[j++] = cmd[i];
            }
            j = 0;

#ifdef BEM280_TEST
         rui_temperature_get(&temp);
         NRF_LOG_INFO("temperature = %d\r\n",temp);
         rui_humidity_get(&humidity);
         NRF_LOG_INFO("humidity = %d\r\n",humidity);
         rui_pressure_get(&pressure);
         NRF_LOG_INFO("pressure = %d\r\n",pressure);
#endif

#ifdef LPS22HB_TEST
         rui_pressure_get(&pressure);
         NRF_LOG_INFO("pressure = %d hPa\r\n",pressure); 
#endif
#ifdef LIS3DH_TEST
         rui_acceleration_get(&x,&y,&z);
         NRF_LOG_INFO("acceleration x,y,z = %d mg,%d mg,%d mg",x,y,z);

#endif
#ifdef LIS2MDL_TEST
         rui_magnetic_get(&magnetic_x,&magnetic_y,&magnetic_z);
         NRF_LOG_INFO("magnetic x,y,z = %d,%d,%d\r\n",magnetic_x,magnetic_y,magnetic_z);
#endif
#ifdef OPT3001_TEST
         rui_light_strength_get(&light);
         NRF_LOG_INFO("light strength = %d\r\n",light);
#endif

#if defined(SHT31_TEST) || defined(SHTC3_TEST)
         rui_temperature_get(&temp);
         NRF_LOG_INFO("temperature = %d\r\n",temp);
         rui_humidity_get(&humidity);
         NRF_LOG_INFO("humidity = %d\r\n",humidity);
#endif

         rui_gps_info_get(gps_data,128);
         delay_ms(500);
		 NRF_LOG_INFO("GPS = %s\r\n",gps_data);
         memset(test_data,0,256);
         sensor_len = sprintf(test_data,"Acc:%d,%d,%d;Tem:%d;Hum:%d;Pre:%d;Mag:%d,%d,%d;Lig:%d;Gps:%s;",x,y,z,(int)temp,(int)humidity,(int)pressure,(int)magnetic_x,(int)magnetic_y,(int)magnetic_z,(int)light,gps_data);
         memset(cmd,0,128);
         hologram_cmd_packet(device_key,test_data);
         NRF_LOG_INFO("device_key = %s\r\n",device_key);
         NRF_LOG_INFO("test_data = %s\r\n",test_data);
         NRF_LOG_INFO("test_data len = %d\r\n",sensor_len);                 
         NRF_LOG_INFO("send packet = %s\r\n",cmd);  
         rui_lte_send("AT+QIOPEN=1,0,\"TCP\",\"cloudsocket.hologram.io\",9999,0,1");
         memset(rsp, 0, 500);
         rui_lte_response(rsp, 500, 500 * 60);
         memset(rsp, 0, 500);
         rui_lte_response(rsp, 500, 500 * 20);
         delay_ms(500);
         memset(len,0,20);
         sprintf(len,"AT+QISEND=0,%d",36+sensor_len+1);
         rui_lte_send(len);
         delay_ms(500);                              
         rui_lte_send(cmd);
         memset(rsp, 0, 500);
         rui_lte_response(rsp, 500, 500 * 60);
         memset(rsp, 0, 500);
         rui_lte_response(rsp, 500, 500 * 80);
         rui_lte_send("AT+QICLOSE=0,30000");
         memset(rsp, 0, 500);
         rui_lte_response(rsp, 500, 500 * 60);
      }
      else
      {
         rui_lte_send(cmd);
         memset(rsp, 0, 500);
         rui_lte_response(rsp, 500, 500 * 60);
      }

         memset(cmd,0,128);
         memset(device_key,0,9);
         memset(test_data,0,256);
		 memset(gps_data,0,256);
         memset(len,0,20);
         sensor_len = 0;
         i = 0;
         j = 0;
}