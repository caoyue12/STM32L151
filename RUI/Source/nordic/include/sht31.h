#ifndef __SHT31_H__
#define __SHT31_H__

uint32_t sht31_init();
bool Sht31_readMeasurement_ft(float* humi, float* temp);
bool Sht31_startMeasurementHighResolution();
#endif