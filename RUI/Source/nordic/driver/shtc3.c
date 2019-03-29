//==============================================================================
//    S E N S I R I O N   AG,  Laubisruetistr. 50, CH-8712 Staefa, Switzerland
//==============================================================================
// Project   :  SHTC3 Sample Code (V1.0)
// File      :  shtc3.c (V1.0)
// Author    :  RFU
// Date      :  24-Nov-2017
// Controller:  STM32F100RB
// IDE       :  µVision V5.17.0.0
// Compiler  :  Armcc
// Brief     :  Sensor Layer: Implementation of functions for sensor access.
//==============================================================================
#include "board_basic.h"
#include "shtc3.h"
#include "hal_i2c.h"

typedef enum{
  READ_ID            = 0xEFC8, // command: read ID register
  SOFT_RESET         = 0x805D, // soft reset
  SLEEP              = 0xB098, // sleep
  WAKEUP             = 0x3517, // wakeup
  MEAS_T_RH_POLLING  = 0x7866, // meas. read T first, clock stretching disabled
  MEAS_T_RH_CLOCKSTR = 0x7CA2, // meas. read T first, clock stretching enabled
  MEAS_RH_T_POLLING  = 0x58E0, // meas. read RH first, clock stretching disabled
  MEAS_RH_T_CLOCKSTR = 0x5C24  // meas. read RH first, clock stretching enabled
}etCommands;

static uint32_t SHTC3_StartWriteAccess(void);
static uint32_t SHTC3_StartReadAccess(void);
static void SHTC3_StopAccess(void);
static uint32_t SHTC3_Read2BytesAndCrc(uint16_t *data);
static uint32_t SHTC3_WriteCommand(etCommands cmd);
static uint32_t SHTC3_CheckCrc(uint8_t data[], uint8_t nbrOfBytes, uint8_t checksum);
static float SHTC3_CalcTemperature(uint16_t rawValue);
static float SHTC3_CalcHumidity(uint16_t rawValue);

static uint8_t _Address = 0x70;

//------------------------------------------------------------------------------
void SHTC3_Init(){

    const nrf_drv_twi_config_t twi_sht_config =
    {
        .scl                = SHTC3_TWI_SCL_PIN,
        .sda                = SHTC3_TWI_SDA_PIN,
        .frequency          = NRF_TWI_FREQ_400K,
        .interrupt_priority = APP_IRQ_PRIORITY_HIGHEST
    };
    rak_i2c_deinit();
    (void)rak_i2c_init(&twi_sht_config);
}

//------------------------------------------------------------------------------
uint32_t SHTC3_GetTempAndHumi(float *temp, float *humi){
  uint32_t  error;        // error code
  uint16_t rawValueTemp; // temperature raw value from sensor
  uint16_t rawValueHumi; // humidity raw value from sensor

  // measure, read temperature first, clock streching enabled
  error |= SHTC3_WriteCommand(MEAS_T_RH_CLOCKSTR);

  // if no error, read temperature and humidity raw values
  if(error == NRF_SUCCESS) {
    error |= SHTC3_Read2BytesAndCrc(&rawValueTemp);
    error |= SHTC3_Read2BytesAndCrc(&rawValueHumi);
  }

  SHTC3_StopAccess();

  // if no error, calculate temperature in °C and humidity in %RH
  if(error == NRF_SUCCESS) {
    *temp = SHTC3_CalcTemperature(rawValueTemp);
    *humi = SHTC3_CalcHumidity(rawValueHumi);
  }

  return error;
}

//------------------------------------------------------------------------------
uint32_t SHTC3_GetTempAndHumiPolling(float *temp, float *humi){
  uint32_t  error;           // error code
  uint8_t  maxPolling = 20; // max. retries to read the measurement (polling)
  uint16_t rawValueTemp;    // temperature raw value from sensor
  uint16_t rawValueHumi;    // humidity raw value from sensor

  // measure, read temperature first, clock streching disabled (polling)
  error |= SHTC3_WriteCommand(MEAS_T_RH_POLLING);

  // if no error, ...
  if(error == NRF_SUCCESS) {

    // if no error, read temperature and humidity raw values
    if(error == NRF_SUCCESS) {
      error |= SHTC3_Read2BytesAndCrc(&rawValueTemp);
      error |= SHTC3_Read2BytesAndCrc(&rawValueHumi);
    }
  }

  // if no error, calculate temperature in °C and humidity in %RH
  if(error == NRF_SUCCESS) {
    *temp = SHTC3_CalcTemperature(rawValueTemp);
    *humi = SHTC3_CalcHumidity(rawValueHumi);
  }

  return error;
}

//------------------------------------------------------------------------------
uint32_t SHTC3_GetId(uint16_t *id){
  uint32_t error; // error code

  // write ID read command
  error |= SHTC3_WriteCommand(READ_ID);

  // if no error, read ID
  if(error == NRF_SUCCESS) {
    error = SHTC3_Read2BytesAndCrc(id);
  }

  return error;
}

//------------------------------------------------------------------------------
uint32_t SHTC3_Sleep(void) {
    uint32_t error; // error code

  if(error == NRF_SUCCESS) {
    error |= SHTC3_WriteCommand(SLEEP);
  }

  return error;
}

//------------------------------------------------------------------------------
uint32_t SHTC3_Wakeup(void) {
    uint32_t error; // error code

  if(error == NRF_SUCCESS) {
    error |= SHTC3_WriteCommand(WAKEUP);
  }

  delay_ms(100); // wait 100 us

  return error;
}

//------------------------------------------------------------------------------
uint32_t SHTC3_SoftReset(void){
  uint32_t error; // error code
  // write reset command
  error |= SHTC3_WriteCommand(SOFT_RESET);
  return error;
}

//------------------------------------------------------------------------------
static uint32_t SHTC3_WriteCommand(etCommands cmd){
  uint32_t error; // error code

  uint8_t tx[2] = {(cmd >> 8), cmd & 0xFF};
  error = rak_i2c_simple_write(_Address, tx, 2);

  return error;
}

//------------------------------------------------------------------------------
static uint32_t SHTC3_Read2BytesAndCrc(uint16_t *data){
  uint32_t error;    // error code
  uint8_t bytes[2]; // read data array
  uint8_t checksum; // checksum byte
  uint8_t rx[3];

  error = rak_i2c_simple_read(_Address, rx, 3);

  // read two data bytes and one checksum byte
  bytes[0] = rx[0];
  bytes[1] = rx[1];
  checksum = rx[2];

  // verify checksum
  error = SHTC3_CheckCrc(bytes, 2, checksum);

  // combine the two bytes to a 16-bit value
  *data = (bytes[0] << 8) | bytes[1];

  return error;
}

//------------------------------------------------------------------------------
static uint32_t SHTC3_CheckCrc(uint8_t data[], uint8_t nbrOfBytes,
                              uint8_t checksum){
  uint8_t bit;        // bit mask
  uint8_t crc = 0xFF; // calculated checksum
  uint8_t byteCtr;    // byte counter

  // calculates 8-Bit checksum with given polynomial
  for(byteCtr = 0; byteCtr < nbrOfBytes; byteCtr++) {
    crc ^= (data[byteCtr]);
    for(bit = 8; bit > 0; --bit) {
      if(crc & 0x80) {
        crc = (crc << 1) ^ CRC_POLYNOMIAL;
      } else {
        crc = (crc << 1);
      }
    }
  }

  // verify checksum
  if(crc != checksum) {
    return 1;
  } else {
    return NRF_SUCCESS;
  }
}

//------------------------------------------------------------------------------
static float SHTC3_CalcTemperature(uint16_t rawValue){
  // calculate temperature [°C]
  // T = -45 + 175 * rawValue / 2^16
  return 175 * (float)rawValue / 65536.0f - 45.0f;
}

//------------------------------------------------------------------------------
static float SHTC3_CalcHumidity(uint16_t rawValue){
  // calculate relative humidity [%RH]
  // RH = rawValue / 2^16 * 100
  return 100 * (float)rawValue / 65536.0f;
}
