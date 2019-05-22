#include "board_basic.h"
#include "radio.h"
#include "sx1276.h"
#include "sx1276_lora.h"

/*!
 * Flag used to set the RF switch control pins in low power mode when the radio is not active.
 */
static bool RadioIsActive = false;
/*!
 * Radio driver structure initialization
 */
const struct Radio_s Radio =
{
    SX1276Init,
    SX1276GetStatus,
    SX1276SetModem,
    SX1276SetChannel,
    SX1276IsChannelFree,
    SX1276Random,
    SX1276SetRxConfig,
    SX1276SetTxConfig,
    SX1276CheckRfFrequency,
    SX1276GetTimeOnAir,
    SX1276Send,
    SX1276SetSleep,
    SX1276SetStby,
    SX1276SetRx,
    SX1276StartCad,
    SX1276SetTxContinuousWave,
    SX1276ReadRssi,
    SX1276Write,
    SX1276Read,
    SX1276WriteBuffer,
    SX1276ReadBuffer,
    SX1276SetMaxPayloadLength,
    SX1276SetPublicNetwork
};

void sx1276_spi_init()
{
     nrf_drv_spi_config_t spi_sx1276_config = NRF_DRV_SPI_DEFAULT_CONFIG;
     spi_sx1276_config.ss_pin   = RADIO_NSS;
     spi_sx1276_config.miso_pin = RADIO_MISO;
     spi_sx1276_config.mosi_pin = RADIO_MOSI;
     spi_sx1276_config.sck_pin  = RADIO_SCK;

     rak_spi_deinit();
     rak_spi_init(&spi_sx1276_config);
}

Gpio_t ioPin;

void SX1276IoInit( void )
{
    memset(&ioPin,0,sizeof(ioPin));
    memset(&SX1276,0,sizeof(SX1276));
    
    sx1276_spi_init();
    
    GpioInit( &SX1276.Reset, RADIO_RESET, PIN_OUTPUT, PIN_OPEN_DRAIN, PIN_NO_PULL, 1 );
    // Turn On TXCO
    GpioInit( &ioPin, RADIO_TCXO, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, 1 );

    GpioInit( &SX1276.DIO0, RADIO_DIO_0, PIN_INPUT, PIN_PUSH_PULL, PIN_PULL_DOWN, 0 );
    GpioInit( &SX1276.DIO1, RADIO_DIO_1, PIN_INPUT, PIN_PUSH_PULL, PIN_PULL_DOWN, 0 );
    // GpioInit( &SX1276.DIO2, RADIO_DIO_2, PIN_INPUT, PIN_PUSH_PULL, PIN_PULL_DOWN, 0 );
    //GpioInit( &SX1276.DIO3, RADIO_DIO_3, PIN_INPUT, PIN_PUSH_PULL, PIN_PULL_DOWN, 0 );    
}

void SX1276Write( uint8_t addr, uint8_t data )
{
    rak_spi_write(addr | 0x80, &data, 1 );
}

uint8_t SX1276Read( uint8_t addr )
{
    uint8_t data;
    rak_spi_read( addr & 0x7F , &data, 1 );
    return data;
}

void SX1276WriteBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{    
    rak_spi_write( addr | 0x80, buffer, size );
}

void SX1276ReadBuffer( uint8_t addr, uint8_t *buffer, uint8_t size )
{
    rak_spi_read(addr & 0x7F, buffer, size );
}

void SX1276WriteFifo( uint8_t *buffer, uint8_t size )
{
    rak_spi_write( 0 | 0x80, buffer, size );
}

void SX1276ReadFifo( uint8_t *buffer, uint8_t size )
{
    rak_spi_read(0 & 0x7F, buffer, size );
}


void SX1276IoIrqInit( DioIrqHandler **irqHandlers )
{
    GpioSetInterrupt( &SX1276.DIO0, IRQ_RISING_EDGE, IRQ_HIGH_PRIORITY, irqHandlers[0]);
    GpioSetInterrupt( &SX1276.DIO1, IRQ_RISING_EDGE, IRQ_HIGH_PRIORITY, irqHandlers[1] );
    //GpioSetInterrupt( &SX1276.DIO2, IRQ_RISING_EDGE, IRQ_HIGH_PRIORITY, irqHandlers[2] );
    //GpioSetInterrupt( &SX1276.DIO3, IRQ_RISING_EDGE, IRQ_HIGH_PRIORITY, irqHandlers[3] );
}

void SX1276IoDeInit( void )
{
    NRF_LOG_DEBUG("SX1276IoDeInit\r\n");
    //SpiDeInit( &SX1276.Spi );

    GpioDeinit( &SX1276.DIO0 );
    GpioDeinit( &SX1276.DIO1 );
    GpioDeinit( &SX1276.DIO2 );
    GpioDeinit( &SX1276.DIO3 );
    NRF_LOG_DEBUG("IoDeInit done\r\n");
}

void SX1276SetRfTxPower( int8_t power )
{
    uint8_t paConfig = 0;
    uint8_t paDac = 0;

    paConfig = SX1276Read( REG_PACONFIG );
    paDac = SX1276Read( REG_PADAC );

    paConfig = ( paConfig & RF_PACONFIG_PASELECT_MASK ) | SX1276GetPaSelect( SX1276.Settings.Channel );
    paConfig = ( paConfig & RF_PACONFIG_MAX_POWER_MASK ) | 0x70;

    if ( ( paConfig & RF_PACONFIG_PASELECT_PABOOST ) == RF_PACONFIG_PASELECT_PABOOST )
    {
        if ( power > 17 )
        {
            paDac = ( paDac & RF_PADAC_20DBM_MASK ) | RF_PADAC_20DBM_ON;
        }
        else
        {
            paDac = ( paDac & RF_PADAC_20DBM_MASK ) | RF_PADAC_20DBM_OFF;
        }
        if ( ( paDac & RF_PADAC_20DBM_ON ) == RF_PADAC_20DBM_ON )
        {
            if ( power < 5 )
            {
                power = 5;
            }
            if ( power > 20 )
            {
                power = 20;
            }
            paConfig = ( paConfig & RF_PACONFIG_OUTPUTPOWER_MASK ) | ( uint8_t )( ( uint16_t )( power - 5 ) & 0x0F );
        }
        else
        {
            if ( power < 2 )
            {
                power = 2;
            }
            if ( power > 17 )
            {
                power = 17;
            }
            paConfig = ( paConfig & RF_PACONFIG_OUTPUTPOWER_MASK ) | ( uint8_t )( ( uint16_t )( power - 2 ) & 0x0F );
        }
    }
    else
    {
        if ( power < -1 )
        {
            power = -1;
        }
        if ( power > 14 )
        {
            power = 14;
        }
        paConfig = ( paConfig & RF_PACONFIG_OUTPUTPOWER_MASK ) | ( uint8_t )( ( uint16_t )( power + 1 ) & 0x0F );
    }
    SX1276Write( REG_PACONFIG, paConfig );
    SX1276Write( REG_PADAC, paDac );
}

uint8_t SX1276GetPaSelect( uint32_t channel )
{
//    if ( channel < RF_MID_BAND_THRESH )
//    {
//        return RF_PACONFIG_PASELECT_PABOOST;
//    }
//    else
//    {
//        return RF_PACONFIG_PASELECT_RFO;
//    }
     return RF_PACONFIG_PASELECT_PABOOST;
}

void SX1276SetAntSwLowPower( bool status )
{
    if ( RadioIsActive != status )
    {
        RadioIsActive = status;

        if ( status == false )
        {
            SX1276AntSwInit( );
        }
        else
        {
            SX1276AntSwDeInit( );
        }
    }
}

/*!
 * Antenna switch GPIO pins objects
 */
Gpio_t AntSwitchHf_RADIO_RF_CTX;
Gpio_t AntSwitchHf_RADIO_RF_CPS;

void SX1276AntSwInit( void )
{
	// Turn On RF switch
    memset(&AntSwitchHf_RADIO_RF_CTX,0,sizeof(AntSwitchHf_RADIO_RF_CTX));
    memset(&AntSwitchHf_RADIO_RF_CPS,0,sizeof(AntSwitchHf_RADIO_RF_CPS));
    GpioInit( &AntSwitchHf_RADIO_RF_CTX, RADIO_RF_CTX, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, RF_ANT_INIT );
    GpioInit( &AntSwitchHf_RADIO_RF_CPS, RADIO_RF_CPS, PIN_OUTPUT, PIN_PUSH_PULL, PIN_NO_PULL, RF_ANT_INIT );
}

void SX1276AntSwDeInit( void )
{
    GpioDeinit( &AntSwitchHf_RADIO_RF_CTX );
    GpioDeinit( &AntSwitchHf_RADIO_RF_CPS );
}

void SX1276SetAntSw( uint8_t opMode )
{
    switch( opMode )
    {
    case RFLR_OPMODE_TRANSMITTER:
        GpioWrite( &AntSwitchHf_RADIO_RF_CPS, 1 );
        break;
    case RFLR_OPMODE_RECEIVER:
    case RFLR_OPMODE_RECEIVER_SINGLE:
    case RFLR_OPMODE_CAD:
    default:
        GpioWrite( &AntSwitchHf_RADIO_RF_CPS, 0 );
        break;
    }
}

bool SX1276CheckRfFrequency( uint32_t frequency )
{
    // Implement check. Currently all frequencies are supported
    return true;
}
