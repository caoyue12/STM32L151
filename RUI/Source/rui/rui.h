#ifndef _RUI_H
#define _RUI_H

#include "stdint.h"
#include "string.h"
#include "stdbool.h"
#include "stddef.h"

//rui device mode
typedef enum DRIVER_MODE
{
	NORMAL_MODE = 0,
	POWER_ON_MODE,
	POWER_OFF_MODE,
	SLEEP_MODE,
	STANDBY_MODE
}DRIVER_MODE;

/*!
 * RUI API return value
 */
#ifndef SUCCESS
#define SUCCESS		1
#endif

#ifndef FAIL
#define FAIL		0
#endif

uint32_t rui_temperature_get(double *temp);
uint32_t rui_temperature_mode_set(DRIVER_MODE mode);
uint32_t rui_humidity_get(double *humidity);
uint32_t rui_humidity_mode_set(DRIVER_MODE mode);
uint32_t rui_pressure_get(double *pressure);
uint32_t rui_pressure_mode_set(DRIVER_MODE mode);
uint32_t rui_acceleration_get(int *x, int *y, int *z);
uint32_t rui_acceleration_mode_set(DRIVER_MODE mode);
uint32_t rui_magnetic_get(float *magnetic_x, float *magnetic_y, float *magnetic_z);
uint32_t rui_magnetic_mode_set(DRIVER_MODE mode);
uint32_t rui_light_strength_get(float *light_data);
uint32_t rui_light_strength_mode_set(DRIVER_MODE mode);
uint32_t rui_gps_info_get(uint8_t *data, uint32_t len);
uint32_t rui_gps_mode_set(DRIVER_MODE mode);
uint32_t rui_lte_send(uint8_t *cmd);
uint32_t rui_lte_response(uint8_t *rsp, uint32_t len, uint32_t timeout);
uint32_t rui_lte_mode_set(DRIVER_MODE mode);
uint32_t rui_lora_send(uint8_t *cmd);
uint32_t rui_lora_response(uint8_t *rsp, uint32_t len, uint32_t timeout);
uint32_t rui_lora_mode_set(DRIVER_MODE mode);

#endif

