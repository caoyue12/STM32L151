
#ifndef __PIN_DEFINE_LORA815_H__
#define __PIN_DEFINE_LORA815_H__


#define RESET_PIN       P21
/**
 * Pin definitions
 */
#define RADIO_DIO_0		P27
#define RADIO_DIO_1		P28
#define RADIO_DIO_2		P29
#define RADIO_DIO_3		P30
#define RADIO_DIO_4		P31


#define RADIO_NSS		P4
#define RADIO_MOSI		P5
#define RADIO_MISO		P6
#define RADIO_SCK		P7

#define RADIO_RESET		P3
#define RADIO_TCXO		P5
#define RADIO_RF_CTX	        P15
#define RADIO_RF_CPS	        P16


#define RF_ANT_INIT		0


#define ASSERT_ERROR	0xA55EA55E

#define USE_FULL_ASSERT
#ifdef  USE_FULL_ASSERT
/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr: If expr is false, it calls assert_failed function
  *         which reports the name of the source file and the source
  *         line number of the call that failed.
  *         If expr is true, it returns no value.
  * @retval None
  */
	#define assert_param(expr)	((expr) ? (void)0U : app_error_handler(ASSERT_ERROR, __LINE__, (const uint8_t *)__FILE__))
#else
	#define assert_param(expr) ((void)0U)
#endif /* USE_FULL_ASSERT */

	
    /*
            UART PIN Assignment
            UART_TXD_PIN        --  P0.28
            UART_RXD_PIN        --  P0.29
    
    */
    
#define             UART_TXD_PIN                        29
#define             UART_RXD_PIN                        28
#define             UART_RTS_PIN                        0
#define             UART_CTS_PIN                        0



/*
		I2C PIN Assignment
		
		lis3dh,sht31,oled,gps
		SCL		--	P0.16
		SDA		--	P0.15

*/
#define             DEV_TWI_SCL_PIN                        16
#define             DEV_TWI_SDA_PIN                        15


/*
		GPS PIN Assignment
		GPS_STANDBY		--	P0.07
		GPS_TXD			--	P0.08
		GPS_RXD		--	P0.09(nfc default)
		GPS_PWR_ON		--	P0.31
		GPS_RESET		--	P0.31

*/
#define             GPS_STANDBY_PIN                    7
#define             GPS_TXD_PIN                        8
#define             GPS_RXD_PIN                        9
#define             GPS_TWI_SCL_PIN                        16
#define             GPS_TWI_SDA_PIN                        15
#define 	         GPS_PWR_ON_PIN	        	31
#define             GPS_RESET_PIN                        31
    
#define             GPS_PWR_ON                     nrf_gpio_pin_write ( GPS_PWR_ON_PIN, 1 )
#define             GPS_PWR_OFF                      nrf_gpio_pin_write ( GPS_PWR_ON_PIN, 0 )
    
#define             GPS_RESET_HIGH                           nrf_gpio_pin_write ( GPS_RESET_PIN, 1 )
#define             GPS_RESET_LOW                            nrf_gpio_pin_write ( GPS_RESET_PIN, 0 )



/*
		lis3dh PIN Assignment
		LIS3DH_SCL		--	P0.18
		LIS3DH_SDA		--	P0.19
		LIS3DH_INT1		--	P0.25
		LIS3DH_RES		--	P0.26
		LIS3DH_INT2		--	P0.27
		
*/
#define             LIS3DH_TWI_SCL_PIN                        16
#define             LIS3DH_TWI_SDA_PIN                        15
#define             LIS3DH_INT1_PIN                        4
#define 		  LIS3DH_RES_PIN				26
#define             LIS3DH_INT2_PIN                       3
#define             LIS3DH_INT1_THRESHOLD          1


#endif  // __PIN_DEFINE_LORA815_H__
