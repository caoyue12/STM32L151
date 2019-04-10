#ifndef _RUI_H
#define _RUI_H

#include "stdint.h"
#include "string.h"
#include "stdbool.h"
#include "stddef.h"

/*******************************************************************************************
 RUI provides a series of common api for user to accomplish their feature. Every driver must
 ba as a instantiation of rui struct to be a sheild between app and driver. BSP driver will  
 does according to the device name. For example, if app calls rui_function.get("lis3dh"),
 the acc will return x,y,z data. Below is the driver names support now:

 NB-IOT:bc95-g,bg96,m35
 LoRa:sx1276
 GPS:L_70_R,ublox-max7,bg96
 Sensor:bme280,lis2mdl,lis3dh,opt3001,sht31,shtc3,lps22hb

 Do not change this file!!!
********************************************************************************************/
#define RUI_DRIVER_NUM			50

enum DRIVER_MODE
{
	NORMAL_MODE = 0,
	POWER_ON_MODE,
	POWER_OFF_MODE,
	SLEEP_MODE,
	STANDBY_MODE
}DRIVER_MODE;

typedef void (*interrupt_callback)(void);

typedef struct rui_function_stru
{
	uint8_t  driver_name[30];											//driver name string
	bool     (*start)(uint8_t *device);		    						//driver begin work
	bool     (*suspend)(uint8_t *device);								//driver suspend work
	bool     (*resume)(uint8_t *device);								//driver resume work
	bool  	 (*stop)(uint8_t *device);		    						//driver stop work
	bool  	 (*mode_switch)(uint8_t *device,DRIVER_MODE mode);		    //driver mode switch
	bool	 (*get)(uint8_t *device,(void *)data);	                    //get data from sensor like gps, light strength
	bool     (*accuracy_set)(uint8_t *device,(void *)data);				//set accuracy for sensor  
    bool     (*threshold_set)(uint8_t *device,(void *)data);			//set threshold for sensor
    bool     (*get_device_id)(uint8_t *device,(void *)data);			//get device info
    bool     (*reset)(uint8_t *device);		                    		//reset device
    bool     (*send)(uint8_t *device,(void *)data);                     //communicate for nb-iot or lora
	void     (*interrupt_call_back1)(void);					//interrupt1 callback for app
	void     (*interrupt_call_back2)(void);					//interrupt2 callback for app
}rui_function_stru;

//for app call, like rui_function.get("lis3dh")
rui_function_stru rui_function;

//for app register theri interrupt callback and remain 2 callbacks for each driver
void rui_interrupt_register(uint8_t *device,interrupt_callback callback);

//app rui function init
void rui_function_init(void);
#endif

