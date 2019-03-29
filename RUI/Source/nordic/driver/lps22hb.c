
/* -----------------------------------------------------------------------------
*                                          Includes
* ------------------------------------------------------------------------------
*/
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "nrf_drv_twi.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include "app_util_platform.h"
#include "board_basic.h"
#include "hal_i2c.h"
#include "lps22hb.h"
#include "math.h"
/* -----------------------------------------------------------------------------
*                                           Constants
* ------------------------------------------------------------------------------
*/
#define LPS22HB_ADDR          0x5C
/* Register addresses */
#define REG_CTRL1                      0x10
#define REG_CTRL2                      0x11
#define REG_PRE_XL		       0x28
#define REG_PRE_L		              0x29
#define REG_PRE_H                    0x2A


uint32_t lps22hb_twi_init(void)
{
    uint32_t err_code;

    const nrf_drv_twi_config_t twi_config =
    {
        .scl                = LPS22HB_TWI_SCL_PIN,
        .sda                = LPS22HB_TWI_SDA_PIN,
        .frequency          = NRF_TWI_FREQ_400K,
        .interrupt_priority = APP_IRQ_PRIORITY_HIGHEST
    };

    rak_i2c_deinit();
    err_code = rak_i2c_init(&twi_config);
    if(err_code != NRF_SUCCESS)
    {
        return err_code;
    }

    return NRF_SUCCESS;
}

/**************************************************************************************************
 * @fn          sensorReadReg
 *
 * @brief       This function implements the I2C protocol to read from a sensor. The sensor must
 *              be selected before this routine is called.
 *
 * @param       addr - which register to read
 * @param       pBuf - pointer to buffer to place data
 * @param       nBytes - numbver of bytes to read
 *
 * @return      TRUE if the required number of bytes are received
 **************************************************************************************************/
bool sensorReadReg(uint8_t addr, uint8_t *pBuf, uint8_t nBytes)
{
    if(rak_i2c_read(LPS22HB_ADDR, addr,pBuf,nBytes) == NRF_SUCCESS)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**************************************************************************************************
* @fn          sensorWriteReg
* @brief       This function implements the I2C protocol to write to a sensor. he sensor must
*              be selected before this routine is called.
*
* @param       addr - which register to write
* @param       pBuf - pointer to buffer containing data to be written
* @param       nBytes - number of bytes to write
*
* @return      TRUE if successful write
*/
bool sensorWriteReg(uint8_t addr, uint8_t *pBuf, uint8_t nBytes)
{
    if(rak_i2c_write(LPS22HB_ADDR, addr, pBuf, nBytes) == NRF_SUCCESS)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int lps22hb_init(void)
{
    bool ret;
    uint8_t cmd = 0x40; 

    ret = sensorWriteReg(REG_CTRL1, &cmd,1);
    if(!ret)
    {
        NRF_LOG_INFO(LOG_INFO, "olps22hb_init fail\r\n");
    }
    else
    {
        return -1;
    }
    return 0;
}

int get_lps22hb_data(float *pressure_data)
{
    uint8_t p_xl = 0;
    uint8_t p_l = 0;
    uint8_t p_h = 0;
    int32_t tmp = 0;
    bool ret;
    ret = sensorReadReg(REG_PRE_XL, &p_xl,1);
    ret = sensorReadReg(REG_PRE_L, &p_l,1);
    ret = sensorReadReg(REG_PRE_H, &p_h,1);
    tmp = ((uint32_t)p_h << 16) | ((uint32_t)p_l << 8) | p_xl;
    if(tmp & 0x00800000)
    {
        tmp |= 0xFF000000;
    }
    *pressure_data = tmp / 4096.00;
    return ret;
}

void lps22hb_deinit()
{
    rak_i2c_deinit();
}


