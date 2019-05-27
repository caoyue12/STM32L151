/*
/ _____)             _              | |
( (____  _____ ____ _| |_ _____  ____| |__
\____ \| ___ |    (_   _) ___ |/ ___)  _ \
_____) ) ____| | | || |_| ____( (___| | | |
(______/|_____)_|_|_| \__)_____)\____)_| |_|
(C)2013 Semtech

Description: LoRaMac classA device implementation

License: Revised BSD License, see LICENSE.TXT file include in the project

Maintainer: Miguel Luis and Gregory Cristian
*/

/*! \file classA/SensorNode/main.c */

#include <stdint.h>
#include <string.h>
#include <stddef.h>
#include <math.h>
#include "board_basic.h"
#include "utils.h"
#include "LoRaMac.h"
#include "Region.h"
#include "Commissioning.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_delay.h"
#include "utils.h"
#include "nrf_log_default_backends.h"
#include "nrf_fstorage.h"
#include "app_error.h"
#include "nrf_soc.h"
#include "nrf_sdh.h"
#include "nrf_nvmc.h"

/*!
* Defines the application data transmission duty cycle. 5s, value in [ms].
*/
#define APP_TX_DUTYCYCLE                            20000

/*!
* Defines a random delay for application data transmission duty cycle. 1s,
* value in [ms].
*/
#define APP_TX_DUTYCYCLE_RND                        1000

/*!
* Default datarate
*/
#define LORAWAN_DEFAULT_DATARATE                    DR_0

/*!
* LoRaWAN confirmed messages
*/
#define LORAWAN_CONFIRMED_MSG_ON                    false

/*!
* LoRaWAN Adaptive Data Rate
*
* \remark Please note that when ADR is enabled the end-device should be static
*/
#define LORAWAN_ADR_ON                              1

#if defined( REGION_EU868 )

#include "LoRaMacTest.h"

uint8_t g_lora_join_success = 0;

/*!
* LoRaWAN ETSI duty cycle control enable/disable
*
* \remark Please note that ETSI mandates duty cycled transmissions. Use only for test purposes
*/
#define LORAWAN_DUTYCYCLE_ON                        true

//#define USE_SEMTECH_DEFAULT_CHANNEL_LINEUP          1

#if( USE_SEMTECH_DEFAULT_CHANNEL_LINEUP == 1 )

#define LC4                { 867100000, 0, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC5                { 867300000, 0, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC6                { 867500000, 0, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC7                { 867700000, 0, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC8                { 867900000, 0, { ( ( DR_5 << 4 ) | DR_0 ) }, 0 }
#define LC9                { 868800000, 0, { ( ( DR_7 << 4 ) | DR_7 ) }, 2 }
#define LC10               { 868300000, 0, { ( ( DR_6 << 4 ) | DR_6 ) }, 1 }

#endif

#endif

/*!
* LoRaWAN application port
*/
#define LORAWAN_APP_PORT                            2

/*!
* User application data buffer size
*/
#if defined( REGION_CN470 ) || defined( REGION_CN779 ) || defined( REGION_EU433 ) || defined( REGION_EU868 ) || defined( REGION_IN865 ) || defined( REGION_KR920 )

#define LORAWAN_APP_DATA_SIZE                       16

#elif defined( REGION_AS923 ) || defined( REGION_AU915 ) || defined( REGION_US915 ) || defined( REGION_US915_HYBRID )

#define LORAWAN_APP_DATA_SIZE                       11

#else

#error "Please define a region in the compiler options."

#endif

static uint8_t DevEui[] = LORAWAN_DEVICE_EUI;
static uint8_t AppEui[] = LORAWAN_APPLICATION_EUI;
static uint8_t AppKey[] = LORAWAN_APPLICATION_KEY;

#if( OVER_THE_AIR_ACTIVATION == 0 )

static uint8_t NwkSKey[] = LORAWAN_NWKSKEY;
static uint8_t AppSKey[] = LORAWAN_APPSKEY;

/*!
* Device address
*/
static uint32_t DevAddr = LORAWAN_DEVICE_ADDRESS;

#endif

/*!
* Application port
*/
static uint8_t AppPort = LORAWAN_APP_PORT;

/*!
* User application data size
*/
static uint8_t AppDataSize = LORAWAN_APP_DATA_SIZE;

/*!
* User application data buffer size
*/
#define LORAWAN_APP_DATA_MAX_SIZE                           100

/*!
* User application data
*/
static uint8_t AppData[LORAWAN_APP_DATA_MAX_SIZE];


/*!
* Indicates if the node is sending confirmed or unconfirmed messages
*/
static uint8_t IsTxConfirmed = LORAWAN_CONFIRMED_MSG_ON;

/*!
* Defines the application data transmission duty cycle
*/
static uint32_t TxDutyCycleTime;

/*!
* Timer to handle the application data transmission duty cycle
*/
static TimerEvent_t TxNextPacketTimer;

/*!
* Specifies the state of the application LED
*/
static bool AppLedStateOn = false;


/*!
* Indicates if a new packet can be sent
*/
static bool NextTx = true;


extern void cli_init(void);
extern void cli_start(void);
extern void cli_process(void);
void reset_handle(void);

/*!
* Device states
*/
static enum eDeviceState
{
    DEVICE_STATE_INIT,
    DEVICE_STATE_JOIN,
    DEVICE_STATE_SEND,
    DEVICE_STATE_CYCLE,
    DEVICE_STATE_SLEEP
}DeviceState;

/*!
* LoRaWAN compliance tests support data
*/
struct ComplianceTest_s
{
    bool Running;
    uint8_t State;
    bool IsTxConfirmed;
    uint8_t AppPort;
    uint8_t AppDataSize;
    uint8_t *AppDataBuffer;
    uint16_t DownLinkCounter;
    bool LinkCheck;
    uint8_t DemodMargin;
    uint8_t NbGateways;
}ComplianceTest;



lora_cfg_t g_lora_cfg_t;

uint32_t LORA_CONFIG_START_ADDRESS = 0x0007F000;
void dump_hex2str(uint8_t *buf , uint8_t len);

void lora_region_print()
{
#if defined( REGION_AS923 )
	NRF_LOG_INFO("Selected LoraWAN 1.0.2 Region: REGION_AS923 \r\n");    
#elif defined( REGION_AU915 )
	NRF_LOG_INFO("Selected LoraWAN 1.0.2 Region: REGION_AU915 \r\n");    
#elif defined( REGION_CN470 )
	NRF_LOG_INFO("Selected LoraWAN 1.0.2 Region: REGION_CN470 \r\n"); 
#elif defined( REGION_CN779 )
	NRF_LOG_INFO("Selected LoraWAN 1.0.2 Region: REGION_CN779 \r\n"); 
#elif defined( REGION_EU433 )
	NRF_LOG_INFO("Selected LoraWAN 1.0.2 Region: REGION_EU433 \r\n");
#elif defined( REGION_EU868 )
	NRF_LOG_INFO("Selected LoraWAN 1.0.2 Region: REGION_EU868 \r\n");
#elif defined( REGION_IN865 )
	NRF_LOG_INFO("Selected LoraWAN 1.0.2 Region: REGION_IN865 \r\n");
#elif defined( REGION_KR920 )
	NRF_LOG_INFO("Selected LoraWAN 1.0.2 Region: REGION_KR920 \r\n");
#elif defined( REGION_US915 )
	NRF_LOG_INFO("Selected LoraWAN 1.0.2 Region: REGION_US915 \r\n");
#elif defined( REGION_US915_HYBRID )
	NRF_LOG_INFO("Selected LoraWAN 1.0.2 Region: REGION_US915_HYBRID \r\n");
#else
#error "Please define a region in the compiler options."
#endif

}
void read_lora_config(void)
{
	   lora_region_print();
#if( OVER_THE_AIR_ACTIVATION != 0 )

       memcpy(&g_lora_cfg_t,LORA_CONFIG_START_ADDRESS,sizeof(g_lora_cfg_t));
       memcpy(DevEui,g_lora_cfg_t.dev_eui,sizeof(DevEui));
       memcpy(AppEui,g_lora_cfg_t.app_eui,sizeof(AppEui));
       memcpy(AppKey,g_lora_cfg_t.app_key,sizeof(AppKey)); 
       
       NRF_LOG_INFO("Lora OTAA config: \r\n");
       NRF_LOG_INFO("Dev_EUI:");
       dump_hex2str(DevEui, 8);
       NRF_LOG_INFO("AppEui:");
       dump_hex2str(AppEui , 8);
       NRF_LOG_INFO("AppKey:");
       dump_hex2str(AppKey , 16);
#else
       memcpy(&g_lora_cfg_t,LORA_CONFIG_START_ADDRESS,sizeof(g_lora_cfg_t));
       memcpy(NwkSKey,g_lora_cfg_t.nwkskey,sizeof(NwkSKey));    
       memcpy(AppSKey,g_lora_cfg_t.appskey,sizeof(AppSKey));
       memcpy(DevAddr,g_lora_cfg_t.dev_addr,sizeof(DevAddr)); 
       NRF_LOG_INFO("Lora ABP config: \r\n");
       NRF_LOG_INFO("Dev_EUI: ");
       dump_hex2str(DevEui , 8);
       NRF_LOG_INFO("DevAddr: %08X\r\n", DevAddr);
       NRF_LOG_INFO("NwkSKey: ");
       dump_hex2str(NwkSKey , 16);
       NRF_LOG_INFO("AppSKey: ");
       dump_hex2str(AppSKey , 16);

#endif
}

void write_lora_config(void)
{
    nrf_sdh_disable_request();
    delay_ms(10);
    nrf_nvmc_page_erase(LORA_CONFIG_START_ADDRESS);
    delay_ms(100);
    g_lora_cfg_t.sof = LORA_CONFIG_MAGIC;
    nrf_nvmc_write_bytes(LORA_CONFIG_START_ADDRESS,(uint8_t*)&g_lora_cfg_t,sizeof(g_lora_cfg_t));
    delay_ms(100);
    reset_handle();
}

void dump_hex2str(uint8_t *buf , uint8_t len)
{
    uint8_t str[56] = {0};
    
    for(uint8_t i=0; i<len; i++) {
        sprintf(str+i*2,"%02X",buf[i]);
    }
    NRF_LOG_INFO("%s\r\n",str);
}

int set_handler(int argc, char* argv[], lora_cfg_t* cfg_info)
{
    
    NRF_LOG_INFO("%s=%s", argv[0], argv[1]);
    
    if (strcmp(argv[0], "dev_eui") == 0) {
        StrToHex((char *)cfg_info->dev_eui, argv[1], 8);
    }
    else if (strcmp(argv[0], "app_eui") == 0) {
        StrToHex((char *)cfg_info->app_eui, argv[1], 8);
    }
    else if (strcmp(argv[0], "app_key") == 0) {
        StrToHex((char *)cfg_info->app_key, argv[1], 16);
    }
    else if (strcmp(argv[0], "dev_addr") == 0) {
        cfg_info->dev_addr = strtoul(argv[1], NULL, 16);
    }
    else if (strcmp(argv[0], "nwkskey") == 0) {
        StrToHex((char *)cfg_info->nwkskey, argv[1], 16);
    }
    else if (strcmp(argv[0], "appskey") == 0) {
        StrToHex((char *)cfg_info->appskey, argv[1], 16);
    }
    else
    {
        return -1;  
    }
    return 0;
}

int  parse_lora_config(char* str, lora_cfg_t *cfg)
{
    int argc;
    char* buf, *temp;
    char* argv[10];
    uint8_t flag = 1;
    
    do{
        argc = 0;
        if ((temp = strchr(str, '&')) != NULL) {
            *temp = '\0';
        }
        else
            flag = 0;
        
        if ((buf = strchr(str, '=')) != NULL) {
            argv[0] = str;
            if (*(buf + 1) == '\0')
                argc = 1;
            else {
                argc = 2;
                argv[1] = buf + 1;
            }
            *buf = '\0';
        }
        else {
            NRF_LOG_INFO("err\r\n");
            return -1;
        }
        if (argc > 0) {  
            if (set_handler(argc, argv, cfg) != 0) {
                return -1;
            }
        }
        else {
            return -1;
        }
        str = ++temp;
        
    } while(flag); 
    
    return 0;
}

static void PrepareTxFrame( uint8_t port )
{
    switch( port )
    {
      case 2:
        {
            //AppDataSize = strlen(AppData);
        }
      case 224:
        if( ComplianceTest.LinkCheck == true )
        {
            ComplianceTest.LinkCheck = false;
            AppDataSize = 3;
            AppData[0] = 5;
            AppData[1] = ComplianceTest.DemodMargin;
            AppData[2] = ComplianceTest.NbGateways;
            ComplianceTest.State = 1;
        }
        else
        {
            switch( ComplianceTest.State )
            {
              case 4:
                ComplianceTest.State = 1;
                break;
              case 1:
                AppDataSize = 2;
                AppData[0] = ComplianceTest.DownLinkCounter >> 8;
                AppData[1] = ComplianceTest.DownLinkCounter;
                break;
            }
        }
        break;
      default:
        break;
    }
}

/*!
* \brief   Prepares the payload of the frame
*
* \retval  [0: frame could be send, 1: error]
*/
static bool SendFrame( void )
{
    McpsReq_t mcpsReq;
    LoRaMacTxInfo_t txInfo;
    
    if( LoRaMacQueryTxPossible( AppDataSize, &txInfo ) != LORAMAC_STATUS_OK )
    {
        // Send empty frame in order to flush MAC commands
        mcpsReq.Type = MCPS_UNCONFIRMED;
        mcpsReq.Req.Unconfirmed.fBuffer = NULL;
        mcpsReq.Req.Unconfirmed.fBufferSize = 0;
        mcpsReq.Req.Unconfirmed.Datarate = LORAWAN_DEFAULT_DATARATE;
    }
    else
    {
        if( IsTxConfirmed == false )
        {
            mcpsReq.Type = MCPS_UNCONFIRMED;
            mcpsReq.Req.Unconfirmed.fPort = AppPort;
            mcpsReq.Req.Unconfirmed.fBuffer = AppData;
            mcpsReq.Req.Unconfirmed.fBufferSize = AppDataSize;
            mcpsReq.Req.Unconfirmed.Datarate = LORAWAN_DEFAULT_DATARATE;
        }
        else
        {
            mcpsReq.Type = MCPS_CONFIRMED;
            mcpsReq.Req.Confirmed.fPort = AppPort;
            mcpsReq.Req.Confirmed.fBuffer = AppData;
            mcpsReq.Req.Confirmed.fBufferSize = AppDataSize;
            mcpsReq.Req.Confirmed.NbTrials = 8;
            mcpsReq.Req.Confirmed.Datarate = LORAWAN_DEFAULT_DATARATE;
        }
    }
    
    if( LoRaMacMcpsRequest( &mcpsReq ) == LORAMAC_STATUS_OK )
    {
        return false;
    }
    return true;
}

/*!
* \brief Function executed on TxNextPacket Timeout event
*/
static void OnTxNextPacketTimerEvent( void )
{
#if 1
    MibRequestConfirm_t mibReq;
    LoRaMacStatus_t status;
    
    TimerStop( &TxNextPacketTimer );
    
    mibReq.Type = MIB_NETWORK_JOINED;
    status = LoRaMacMibGetRequestConfirm( &mibReq );

    if( status == LORAMAC_STATUS_OK )
    {
        if( mibReq.Param.IsNetworkJoined == true )
        {
            DeviceState = DEVICE_STATE_SEND;
            NextTx = true;
        }
        else
        {
            DeviceState = DEVICE_STATE_JOIN;
        }
    }
#endif
}


/*!
* \brief   MCPS-Confirm event function
*
* \param   [IN] mcpsConfirm - Pointer to the confirm structure,
*               containing confirm attributes.
*/
static void McpsConfirm( McpsConfirm_t *mcpsConfirm )
{
    if( mcpsConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
    {
        switch( mcpsConfirm->McpsRequest )
        {
          case MCPS_UNCONFIRMED:
            {
                // Check Datarate
                // Check TxPower
                break;
            }
          case MCPS_CONFIRMED:
            {
                // Check Datarate
                // Check TxPower
                // Check AckReceived
                // Check NbTrials
                break;
            }
          case MCPS_PROPRIETARY:
            {
                break;
            }
          default:
            break;
        }
        
    }
    NextTx = true;
}

/*!
* \brief   MCPS-Indication event function
*
* \param   [IN] mcpsIndication - Pointer to the indication structure,
*               containing indication attributes.
*/
static void McpsIndication( McpsIndication_t *mcpsIndication )
{
    if( mcpsIndication->Status != LORAMAC_EVENT_INFO_STATUS_OK )
    {
        return;
    }
    
    switch( mcpsIndication->McpsIndication )
    {
      case MCPS_UNCONFIRMED:
        {
            break;
        }
      case MCPS_CONFIRMED:
        {
            break;
        }
      case MCPS_PROPRIETARY:
        {
            break;
        }
      case MCPS_MULTICAST:
        {
            break;
        }
      default:
        break;
    }
    
    // Check Multicast
    // Check Port
    // Check Datarate
    // Check FramePending
    // Check Buffer
    // Check BufferSize
    // Check Rssi
    // Check Snr
    // Check RxSlot
    
    if( ComplianceTest.Running == true )
    {
        ComplianceTest.DownLinkCounter++;
    }
    
    if( mcpsIndication->RxData == true )
    {
        switch( mcpsIndication->Port )
        {
          case 1: // The application LED can be controlled on port 1 or 2
          case 2:
            if( mcpsIndication->BufferSize == 1 )
            {
                AppLedStateOn = mcpsIndication->Buffer[0] & 0x01;
            }
            break;
          case 224:
            if( ComplianceTest.Running == false )
            {
                // Check compliance test enable command (i)
                if( ( mcpsIndication->BufferSize == 4 ) &&
                   ( mcpsIndication->Buffer[0] == 0x01 ) &&
                       ( mcpsIndication->Buffer[1] == 0x01 ) &&
                           ( mcpsIndication->Buffer[2] == 0x01 ) &&
                               ( mcpsIndication->Buffer[3] == 0x01 ) )
                {
                    IsTxConfirmed = false;
                    AppPort = 224;
                    AppDataSize = 2;
                    ComplianceTest.DownLinkCounter = 0;
                    ComplianceTest.LinkCheck = false;
                    ComplianceTest.DemodMargin = 0;
                    ComplianceTest.NbGateways = 0;
                    ComplianceTest.Running = true;
                    ComplianceTest.State = 1;
                    
                    MibRequestConfirm_t mibReq;
                    mibReq.Type = MIB_ADR;
                    mibReq.Param.AdrEnable = true;
                    LoRaMacMibSetRequestConfirm( &mibReq );
                    
#if defined( REGION_EU868 )
                    LoRaMacTestSetDutyCycleOn( false );
#endif
                    //GpsStop( );
                }
            }
            else
            {
                ComplianceTest.State = mcpsIndication->Buffer[0];
                switch( ComplianceTest.State )
                {
                  case 0: // Check compliance test disable command (ii)
                    IsTxConfirmed = LORAWAN_CONFIRMED_MSG_ON;
                    AppPort = LORAWAN_APP_PORT;
                    AppDataSize = LORAWAN_APP_DATA_SIZE;
                    ComplianceTest.DownLinkCounter = 0;
                    ComplianceTest.Running = false;
                    
                    MibRequestConfirm_t mibReq;
                    mibReq.Type = MIB_ADR;
                    mibReq.Param.AdrEnable = LORAWAN_ADR_ON;
                    LoRaMacMibSetRequestConfirm( &mibReq );
#if defined( REGION_EU868 )
                    LoRaMacTestSetDutyCycleOn( LORAWAN_DUTYCYCLE_ON );
#endif
                    //GpsStart( );
                    break;
                  case 1: // (iii, iv)
                    AppDataSize = 2;
                    break;
                  case 2: // Enable confirmed messages (v)
                    IsTxConfirmed = true;
                    ComplianceTest.State = 1;
                    break;
                  case 3:  // Disable confirmed messages (vi)
                    IsTxConfirmed = false;
                    ComplianceTest.State = 1;
                    break;
                  case 4: // (vii)
                    AppDataSize = mcpsIndication->BufferSize;
                    
                    AppData[0] = 4;
                    for( uint8_t i = 1; i < MIN( AppDataSize, LORAWAN_APP_DATA_MAX_SIZE ); i++ )
                    {
                        AppData[i] = mcpsIndication->Buffer[i] + 1;
                    }
                    break;
                  case 5: // (viii)
                    {
                        MlmeReq_t mlmeReq;
                        mlmeReq.Type = MLME_LINK_CHECK;
                        LoRaMacMlmeRequest( &mlmeReq );
                    }
                    break;
                  case 6: // (ix)
                    {
                        MlmeReq_t mlmeReq;
                        
                        // Disable TestMode and revert back to normal operation
                        IsTxConfirmed = LORAWAN_CONFIRMED_MSG_ON;
                        AppPort = LORAWAN_APP_PORT;
                        AppDataSize = LORAWAN_APP_DATA_SIZE;
                        ComplianceTest.DownLinkCounter = 0;
                        ComplianceTest.Running = false;
                        
                        MibRequestConfirm_t mibReq;
                        mibReq.Type = MIB_ADR;
                        mibReq.Param.AdrEnable = LORAWAN_ADR_ON;
                        LoRaMacMibSetRequestConfirm( &mibReq );
#if defined( REGION_EU868 )
                        LoRaMacTestSetDutyCycleOn( LORAWAN_DUTYCYCLE_ON );
#endif
                        //GpsStart( );
                        
                        mlmeReq.Type = MLME_JOIN;
                        
                        mlmeReq.Req.Join.DevEui = DevEui;
                        mlmeReq.Req.Join.AppEui = AppEui;
                        mlmeReq.Req.Join.AppKey = AppKey;
                        mlmeReq.Req.Join.NbTrials = 3;
                        
                        LoRaMacMlmeRequest( &mlmeReq );
                        DeviceState = DEVICE_STATE_SLEEP;
                    }
                    break;
                  case 7: // (x)
                    {
                        if( mcpsIndication->BufferSize == 3 )
                        {
                            MlmeReq_t mlmeReq;
                            mlmeReq.Type = MLME_TXCW;
                            mlmeReq.Req.TxCw.Timeout = ( uint16_t )( ( mcpsIndication->Buffer[1] << 8 ) | mcpsIndication->Buffer[2] );
                            LoRaMacMlmeRequest( &mlmeReq );
                        }
                        else if( mcpsIndication->BufferSize == 7 )
                        {
                            MlmeReq_t mlmeReq;
                            mlmeReq.Type = MLME_TXCW_1;
                            mlmeReq.Req.TxCw.Timeout = ( uint16_t )( ( mcpsIndication->Buffer[1] << 8 ) | mcpsIndication->Buffer[2] );
                            mlmeReq.Req.TxCw.Frequency = ( uint32_t )( ( mcpsIndication->Buffer[3] << 16 ) | ( mcpsIndication->Buffer[4] << 8 ) | mcpsIndication->Buffer[5] ) * 100;
                            mlmeReq.Req.TxCw.Power = mcpsIndication->Buffer[6];
                            LoRaMacMlmeRequest( &mlmeReq );
                        }
                        ComplianceTest.State = 1;
                    }
                    break;
                  default:
                    break;
                }
            }
            break;
          default:
            break;
        }
    }
    
    // Switch LED 1 ON for each received downlink
    //GpioWrite( &Led1, 0 );
    //TimerStart( &Led1Timer );
}

/*!
* \brief   MLME-Confirm event function
*
* \param   [IN] mlmeConfirm - Pointer to the confirm structure,
*               containing confirm attributes.
*/
static void MlmeConfirm( MlmeConfirm_t *mlmeConfirm )
{
    switch( mlmeConfirm->MlmeRequest )
    {
      case MLME_JOIN:
        {
            if( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
            {
                // Status is OK, node has joined the network
                DeviceState = DEVICE_STATE_SEND;
                NRF_LOG_INFO("OTAA Join Success \r\n");
                g_lora_join_success = 1;
            }
            else
            {
                // Join was not successful. Try to join again
                DeviceState = DEVICE_STATE_JOIN;
            }
            break;
        }
      case MLME_LINK_CHECK:
        {
            if( mlmeConfirm->Status == LORAMAC_EVENT_INFO_STATUS_OK )
            {
                // Check DemodMargin
                // Check NbGateways
                if( ComplianceTest.Running == true )
                {
                    ComplianceTest.LinkCheck = true;
                    ComplianceTest.DemodMargin = mlmeConfirm->DemodMargin;
                    ComplianceTest.NbGateways = mlmeConfirm->NbGateways;
                }
            }
            break;
        }
      default:
        break;
    }
    NextTx = true;
}


LoRaMacPrimitives_t LoRaMacPrimitives;
LoRaMacCallback_t LoRaMacCallbacks;
MibRequestConfirm_t mibReq;




void region_init()
{
	     LoRaMacPrimitives.MacMcpsConfirm = McpsConfirm;
            LoRaMacPrimitives.MacMcpsIndication = McpsIndication;
            LoRaMacPrimitives.MacMlmeConfirm = MlmeConfirm;
            LoRaMacCallbacks.GetBatteryLevel = NULL;
#if defined( REGION_AS923 )
            LoRaMacInitialization( &LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_AS923 );
#elif defined( REGION_AU915 )
            LoRaMacInitialization( &LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_AU915 );
#elif defined( REGION_CN470 )
            LoRaMacInitialization( &LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_CN470 );
#elif defined( REGION_CN779 )
            LoRaMacInitialization( &LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_CN779 );
#elif defined( REGION_EU433 )
            LoRaMacInitialization( &LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_EU433 );
#elif defined( REGION_EU868 )
            LoRaMacInitialization( &LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_EU868 );
#elif defined( REGION_IN865 )
            LoRaMacInitialization( &LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_IN865 );
#elif defined( REGION_KR920 )
            LoRaMacInitialization( &LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_KR920 );
#elif defined( REGION_US915 )
            LoRaMacInitialization( &LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_US915 );
#elif defined( REGION_US915_HYBRID )
            LoRaMacInitialization( &LoRaMacPrimitives, &LoRaMacCallbacks, LORAMAC_REGION_US915_HYBRID );
#else
#error "Please define a region in the compiler options."
#endif
            //TimerInit( &TxNextPacketTimer, OnTxNextPacketTimerEvent );
            
            //TimerInit( &Led1Timer, OnLed1TimerEvent );
            //TimerSetValue( &Led1Timer, 2000 );   
            
            mibReq.Type = MIB_ADR;
            mibReq.Param.AdrEnable = LORAWAN_ADR_ON;
            LoRaMacMibSetRequestConfirm( &mibReq );
            
            mibReq.Type = MIB_PUBLIC_NETWORK;
            mibReq.Param.EnablePublicNetwork = LORAWAN_PUBLIC_NETWORK;
            LoRaMacMibSetRequestConfirm( &mibReq );
            
            //DeviceState = DEVICE_STATE_JOIN;
            NRF_LOG_INFO("goto to join");
#if( OVER_THE_AIR_ACTIVATION != 0 )
            MlmeReq_t mlmeReq;
            
            // Initialize LoRaMac device unique ID
            //BoardGetUniqueId( DevEui );
            
            
            mlmeReq.Type = MLME_JOIN;
            
            mlmeReq.Req.Join.DevEui = DevEui;
            mlmeReq.Req.Join.AppEui = AppEui;
            mlmeReq.Req.Join.AppKey = AppKey;
            mlmeReq.Req.Join.NbTrials = 3;
            //the channel is corresponding to the gateway,this code is according to our server and TTN,
			//so if connect to gateway slowly, change the channel 
#if defined ( REGION_US915 )  
            
            uint16_t ch_mask[6];
            ch_mask[0] =0xFF00;
            ch_mask[1] =0x0000;
            ch_mask[2] =0x0000;
            ch_mask[3] =0x0000;
            ch_mask[4] =0x0000;
            ch_mask[5] =0x0000;      
			
            mibReq.Type = MIB_CHANNELS_DEFAULT_MASK;  
            mibReq.Param.ChannelsDefaultMask = ch_mask;
            LoRaMacMibSetRequestConfirm( &mibReq ); 
            
            mibReq.Type = MIB_CHANNELS_MASK;  
            mibReq.Param.ChannelsDefaultMask = ch_mask;
            LoRaMacMibSetRequestConfirm( &mibReq );
            
#endif 

#if defined ( REGION_CN470 )  
            
            uint16_t ch_mask[6];
            ch_mask[0] =0x0000;
            ch_mask[1] =0x0000;
            ch_mask[2] =0x0000;
            ch_mask[3] =0x0000;
            ch_mask[4] =0x0000;
            ch_mask[5] =0x000F;			
            
            mibReq.Type = MIB_CHANNELS_DEFAULT_MASK;  
            mibReq.Param.ChannelsDefaultMask = ch_mask;
            LoRaMacMibSetRequestConfirm( &mibReq ); 
            
            mibReq.Type = MIB_CHANNELS_MASK;  
            mibReq.Param.ChannelsDefaultMask = ch_mask;
            LoRaMacMibSetRequestConfirm( &mibReq );
            
#endif             
            if( NextTx == true )
            {
                LoRaMacStatus_t status;
                status = LoRaMacMlmeRequest( &mlmeReq );
                NRF_LOG_INFO("OTAA Join Start...%d \r\n", status);
            }

#else
            // Choose a random device address if not already defined in Commissioning.h
            if( DevAddr == 0 )
            {
                // Random seed initialization
                srand1( BoardGetRandomSeed( ) );
                
                // Choose a random device address
                DevAddr = randr( 0, 0x01FFFFFF );
            }

            
            mibReq.Type = MIB_NET_ID;
            mibReq.Param.NetID = LORAWAN_NETWORK_ID;
            LoRaMacMibSetRequestConfirm( &mibReq );
            
            mibReq.Type = MIB_DEV_ADDR;
            mibReq.Param.DevAddr = DevAddr;
            LoRaMacMibSetRequestConfirm( &mibReq );
            
            mibReq.Type = MIB_NWK_SKEY;
            mibReq.Param.NwkSKey = NwkSKey;
            LoRaMacMibSetRequestConfirm( &mibReq );
            
            mibReq.Type = MIB_APP_SKEY;
            mibReq.Param.AppSKey = AppSKey;
            LoRaMacMibSetRequestConfirm( &mibReq );
            
            mibReq.Type = MIB_NETWORK_JOINED;
            mibReq.Param.IsNetworkJoined = true;
            LoRaMacMibSetRequestConfirm( &mibReq );
            
            //DeviceState = DEVICE_STATE_SEND;
#endif
            
}
void app_send()
{
     PrepareTxFrame( 2 );
     NextTx = SendFrame( );
}

uint32_t lora_send(uint8_t *cmd)
{
      uint32_t len = 0;
      memset(AppData,0,LORAWAN_APP_DATA_MAX_SIZE);
      len = sprintf(AppData, "%s", cmd);
      memcpy(AppData,cmd,len);
      AppDataSize = len;
      NRF_LOG_INFO("lora_send len = %d\r\n",len);
      MibRequestConfirm_t mibReq;
      LoRaMacStatus_t status;
      
      mibReq.Type = MIB_NETWORK_JOINED;
      status = LoRaMacMibGetRequestConfirm( &mibReq );
      
      if( status == LORAMAC_STATUS_OK )
      {
          if( mibReq.Param.IsNetworkJoined == true )
          {
              app_send();
          }
      }
      return SUCCESS;
}

Gpio_t ioreset;
GpioIrqHandler *callback;
void reset_handle(void)
{
    NRF_LOG_INFO("Device will Reset after 3s...\r\n");
    delay_ms(1000);
    NRF_LOG_INFO("Device will Reset after 2s...\r\n");
    delay_ms(1000);    
    NRF_LOG_INFO("Device will Reset after 1s...\r\n");
    delay_ms(1000);
    NVIC_SystemReset();
}


void lora_init()
{
    ret_code_t err_code;
    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);
    SX1276IoInit( );
    callback = reset_handle;
    GpioInit(&ioreset,RESET_PIN ,PIN_INPUT,PIN_PUSH_PULL,PIN_PULL_UP,1 );
    GpioSetInterrupt(&ioreset,IRQ_FALLING_EDGE,IRQ_HIGH_PRIORITY,callback);
    read_lora_config();
    if(g_lora_cfg_t.sof == LORA_CONFIG_MAGIC)
    {
        region_init();
    }
}







