#include <stdio.h>
#include "board_basic.h"
#include "gps_max7.h"
#include "hal_i2c.h"
#include "gps.h"
#include "nrf_delay.h"

#define GPS_I2C_ADDR                0x42//( 0x84 ) // GPS IC I2C address
#define NUM_SETUP_COMMANDS          7
#define SETUP_COMMAND_LENGTH        16
#define DATA_STREAM_ADDRESS         0xFF


//MAX7 initialisation commands
const char SetupArray[NUM_SETUP_COMMANDS][SETUP_COMMAND_LENGTH] =
{
    { 0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x29 }, // GxGGA on to I2C
    { 0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x08, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01, 0x09, 0x62 }, // GxZDA on to I2C
    { 0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x2B }, // GxGLL not on the I2C
    { 0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x04, 0x40 }, // GxRMC not on the I2C
    { 0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x05, 0x47 }, // GxVTG not on the I2C
    { 0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x02, 0x32 }, // GxGSA not on the I2C
    { 0xB5, 0x62, 0x06, 0x01, 0x08, 0x00, 0xF0, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x03, 0x39 }  // GxGSV not on the I2C
};

GpsStruct Gps;


bool Max7GpsWriteSetupOK( void );
static uint8_t Max7GpsReadRegister( char thisRegAddress );
bool Max7GpsReadDataStream( void );
	
uint8_t GpsDataBuffer[512] = {0};



bool Max7GpsWriteSetupOK( void )
{
    int lineCount;
    for( lineCount = 0; lineCount < NUM_SETUP_COMMANDS; lineCount++ )
    {
      if( (rak_i2c_simple_write_m( GPS_I2C_ADDR, (uint8_t*)&SetupArray[lineCount][0],SETUP_COMMAND_LENGTH, false)) !=  NRF_SUCCESS)
      {			
          return false;
      }
    }
    return true;
}

void address_read()
{
    uint8_t high = 0;
    uint8_t low = 0;
    uint32_t err_code;
    err_code = rak_i2c_read(GPS_I2C_ADDR, 0xFD, &high, 1);
    if(err_code != NRF_SUCCESS)
    {
        NRF_LOG_INFO("GPS rak_i2c_simple_read err %d\r\n", err_code);
    }
    err_code = rak_i2c_read(GPS_I2C_ADDR, 0xFE, &low, 1);
    if(err_code != NRF_SUCCESS)
    {
        NRF_LOG_INFO("GPS rak_i2c_simple_read err %d\r\n", err_code);
    }
    NRF_LOG_INFO("data num low = %d, high = %d\r\n", low,high);
}

bool Max7GpsReadDataStream( void )
{
    uint8_t incomingCheck;
    bool contFlag = true;
    uint8_t i=0;
    //address_read();
    while( contFlag )
    {
        incomingCheck = Max7GpsReadRegister( DATA_STREAM_ADDRESS );
        if( incomingCheck == 0xFF )
        {
            contFlag = false;
        }
        else
        {
            GpsDataBuffer[i++] = incomingCheck;
        }
    }
    return false;
}


static uint8_t Max7GpsReadRegister( char thisRegAddress )
{
    char thisValue;
    uint8_t retVal;
    uint32_t err_code;
    thisValue = thisRegAddress;
    err_code = rak_i2c_read(GPS_I2C_ADDR, thisRegAddress, (uint8_t*)&thisValue, 1);
    if(err_code != NRF_SUCCESS)
    {
        NRF_LOG_INFO("GPS rak_i2c_simple_read err %d\r\n", err_code);
    }
    retVal = ( uint8_t )thisValue;
    return retVal;
}

void Gps_Gpio_Init()
{
    nrf_gpio_cfg_output(GPS_PWR_ON_PIN);
    nrf_gpio_cfg_output(GPS_RESET_PIN);
}

void gps_setup()
{
    while(!Max7GpsWriteSetupOK());		
}

void gps_poweroff()
{
     GPS_PWR_OFF;
     nrf_delay_ms(1000);   		
}

uint32_t max_init()
{
    uint32_t err_code;

    Gps_Gpio_Init();
    GPS_PWR_OFF;
    nrf_delay_ms(1000);
    GPS_PWR_ON;
    nrf_delay_ms(1000);

    const nrf_drv_twi_config_t twi_max_config =
    {
        .scl                = GPS_TWI_SCL_PIN,
        .sda                = GPS_TWI_SDA_PIN,
        .frequency          = NRF_TWI_FREQ_400K,
        .interrupt_priority = APP_IRQ_PRIORITY_HIGHEST
    };
    err_code = rak_i2c_init(&twi_max_config);
    if(err_code != NRF_SUCCESS)
    {
        return err_code;
    }
    return NRF_SUCCESS;
}


