/*
*********************************************************************************************************
*                                             INCLUDE FILES
*********************************************************************************************************
*/
#include "gps.h"
#include "m35.h"
#include "nrf_drv_rtc.h"
#include "nrf_rtc.h"
#include "nrf_drv_gpiote.h"
#include "hal_uart.h"
#define  GSM_RXBUF_MAXSIZE           1600

static uint16_t rxReadIndex  = 0;
static uint16_t rxWriteIndex = 0;
static uint16_t rxCount      = 0;
static uint8_t Gsm_RxBuf[GSM_RXBUF_MAXSIZE];

const nrf_drv_rtc_t rtc = NRF_DRV_RTC_INSTANCE(2); /**< Declaring an instance of nrf_drv_rtc for RTC0. */

extern GSM_RECEIVE_TYPE g_type;
char GSM_RSP[1600] = {0};


/*
*********************************************************************************************************
*                                         FUNCTION PROTOTYPES
*********************************************************************************************************
*/

uint32_t get_stamp(void)
{
    uint32_t ticks = nrf_drv_rtc_counter_get(&rtc);
    return (ticks / RTC_DEFAULT_CONFIG_FREQUENCY);
}

int GSM_UART_TxBuf(uint8_t *buffer, int nbytes)
{
    uint32_t err_code;
    for (uint32_t i = 0; i < nbytes; i++)
    {
        do
        {
            err_code = app_uart_put(buffer[i]);
            if ((err_code != NRF_SUCCESS) && (err_code != NRF_ERROR_BUSY))
            {
                //NRF_LOG_ERROR("Failed receiving NUS message. Error 0x%x. ", err_code);
                APP_ERROR_CHECK(err_code);
            }
        }
        while (err_code == NRF_ERROR_BUSY);
    }
    return err_code;
}

void Gsm_RingBuf(uint8_t in_data)
{
    Gsm_RxBuf[rxWriteIndex] = in_data;
    rxWriteIndex++;
    rxCount++;

    if (rxWriteIndex == GSM_RXBUF_MAXSIZE)
    {
        rxWriteIndex = 0;
    }

    /* Check for overflow */
    if (rxCount == GSM_RXBUF_MAXSIZE)
    {
        rxWriteIndex = 0;
        rxCount      = 0;
        rxReadIndex  = 0;
    }
}


void Gsm_PowerUp(void)
{
    DPRINTF(LOG_DEBUG, "GMS_PowerUp\r\n");

    GSM_PWR_OFF;
    delay_ms(200);
    /*Pwr en wait at least 30ms*/
    GSM_PWR_ON;
    delay_ms(100);
//		GSM_RESET_LOW;
//		delay_ms(200);
//		GSM_RESET_HIGH;
    /*Pwr key low to high at least 2S*/
    GSM_PWRKEY_LOW;
    delay_ms(1000); //2s
    GSM_PWRKEY_HIGH;
    delay_ms(1000);
}


int Gsm_RxByte(void)
{
    int c = -1;

    __disable_irq();
    if (rxCount > 0)
    {
        c = Gsm_RxBuf[rxReadIndex];

        rxReadIndex++;
        if (rxReadIndex == GSM_RXBUF_MAXSIZE)
        {
            rxReadIndex = 0;
        }
        rxCount--;
    }
    __enable_irq();

    return c;
}


int Gsm_WaitRspOK(char *rsp_value, uint16_t timeout_ms, uint8_t is_rf)
{
    int ret = -1, wait_len = 0;
    char len[10] = {0};
    uint16_t time_count = timeout_ms;
    uint32_t i = 0;
    int       c;
    char *cmp_p = NULL;

    wait_len = is_rf ? strlen(GSM_CMD_RSP_OK_RF) : strlen(GSM_CMD_RSP_OK);

    if(g_type == GSM_TYPE_FILE)
    {
        do
        {
            c = Gsm_RxByte();
            if(c < 0)
            {
                time_count--;
                delay_ms(1);
                continue;
            }

            rsp_value[i++] = (char)c;
            SEGGER_RTT_printf(0, "%02X", rsp_value[i - 1]);
            time_count--;
        }
        while(time_count > 0);
    }
    else
    {
        memset(GSM_RSP, 0, 1600);
        do
        {
            int c;
            c = Gsm_RxByte();
            if(c < 0)
            {
                time_count--;
                delay_ms(1);
                continue;
            }
            //R485_UART_TxBuf((uint8_t *)&c,1);
            SEGGER_RTT_printf(0, "%c", c);
            GSM_RSP[i++] = (char)c;

            if(i >= wait_len)
            {
                if(is_rf)
                    cmp_p = strstr(GSM_RSP, GSM_CMD_RSP_OK_RF);
                else
                    cmp_p = strstr(GSM_RSP, GSM_CMD_RSP_OK);
                if(cmp_p)
                {
                    if(i > wait_len && rsp_value != NULL)
                    {
                        //SEGGER_RTT_printf(0,"--%s  len=%d\r\n", resp, (cmp_p-resp));
                        memcpy(rsp_value, GSM_RSP, (cmp_p - GSM_RSP));
                    }
                    ret = 0;
                    break;
                }
            }
        }
        while(time_count > 0);
    }

    return ret;
}

//The shut off api is emergency cmd. For safe, please use "AT+QPOWD=1" and it will cost less than 60s.
void Gsm_PowerDown(void)
{
    int ret = -1;
    Gsm_print("AT+QPOWD=0");
    memset(GSM_RSP, 0, GSM_GENER_CMD_LEN);
    ret = Gsm_WaitRspOK(GSM_RSP, GSM_GENER_CMD_TIMEOUT * 4, true);
}

int Gsm_WaitSendAck(uint16_t timeout_ms)
{
    int ret = -1;
    uint16_t time_count = timeout_ms;
    do
    {
        int       c;
        c = Gsm_RxByte();
        if(c < 0)
        {
            time_count--;
            delay_ms(1);
            continue;
        }
        //R485_UART_TxBuf((uint8_t *)&c,1);
        if((char)c == '>')
        {
            ret = 0;
            break;
        }
    }
    while(time_count > 0);

    //DPRINTF(LOG_DEBUG,"\r\n");
    return ret;
}

int Gsm_AutoBaud(void)
{
    int ret = -1, retry_num = GSM_AUTO_CMD_NUM;
    //
    char *cmd;

    cmd = (char *)malloc(GSM_GENER_CMD_LEN);
    if(cmd)
    {
        uint8_t cmd_len;
        memset(cmd, 0, GSM_GENER_CMD_LEN);
        cmd_len = sprintf(cmd, "%s\r\n", GSM_AUTO_CMD_STR);
        do
        {
            DPRINTF(LOG_DEBUG, "\r\n auto baud retry\r\n");
            GSM_UART_TxBuf((uint8_t *)cmd, cmd_len);

            ret = Gsm_WaitRspOK(NULL, GSM_GENER_CMD_TIMEOUT, NULL);
            delay_ms(500);
            retry_num--;
        }
        while(ret != 0 && retry_num > 0);

        free(cmd);
    }
    DPRINTF(LOG_DEBUG, "Gsm_AutoBaud ret= %d\r\n", ret);
    return ret;
}

int Gsm_FixBaudCmd(int baud)
{
    int ret = -1;
    char *cmd;

    cmd = (char*)malloc(GSM_GENER_CMD_LEN);
    if(cmd)
    {
        uint8_t cmd_len;
        memset(cmd, 0, GSM_GENER_CMD_LEN);
        cmd_len = sprintf(cmd, "%s%d%s\r\n", GSM_FIXBAUD_CMD_STR, baud, ";&W");
        GSM_UART_TxBuf((uint8_t *)cmd, cmd_len);

        ret = Gsm_WaitRspOK(NULL, GSM_GENER_CMD_TIMEOUT, true);

        free(cmd);
    }
    DPRINTF(LOG_DEBUG, "Gsm_FixBaudCmd ret= %d\r\n", ret);
    return ret;
}

//close cmd echo
int Gsm_SetEchoCmd(int flag)
{
    int ret = -1;
    char *cmd;

    cmd = (char *)malloc(GSM_GENER_CMD_LEN);
    if(cmd)
    {
        uint8_t cmd_len;
        memset(cmd, 0, GSM_GENER_CMD_LEN);
        cmd_len = sprintf(cmd, "%s%d\r\n", GSM_SETECHO_CMD_STR, flag);
        GSM_UART_TxBuf((uint8_t *)cmd, cmd_len);

        ret = Gsm_WaitRspOK(NULL, GSM_GENER_CMD_TIMEOUT, true);

        free(cmd);
    }
    DPRINTF(LOG_DEBUG, "Gsm_SetEchoCmd ret= %d\r\n", ret);
    return ret;
}
//Check SIM Card Status
int Gsm_CheckSimCmd(void)
{
    int ret = -1;
    //
    char *cmd;

    cmd = (char *)malloc(GSM_GENER_CMD_LEN);
    if(cmd)
    {
        uint8_t cmd_len;
        memset(cmd, 0, GSM_GENER_CMD_LEN);
        cmd_len = sprintf(cmd, "%s\r\n", GSM_CHECKSIM_CMD_STR);
        GSM_UART_TxBuf((uint8_t *)cmd, cmd_len);

        memset(cmd, 0, GSM_GENER_CMD_LEN);
        ret = Gsm_WaitRspOK(cmd, GSM_GENER_CMD_TIMEOUT, true);
        DPRINTF(LOG_DEBUG, "Gsm_CheckSimCmd cmd= %s\r\n", cmd);
        if(ret >= 0)
        {
            if(NULL != strstr(cmd, GSM_CHECKSIM_RSP_OK))
            {
                ret = 0;
            }
            else
            {
                ret = -1;
            }
        }


        free(cmd);
    }
    DPRINTF(LOG_DEBUG, "Gsm_CheckSimCmd ret= %d\r\n", ret);
    return ret;
}

void Gsm_print(uint8_t *at_cmd)
{
    uint8_t cmd_len;
    uint8_t CMD[128] = {0};
    if(at_cmd == NULL)
        return;
    memset(CMD, 0, GSM_GENER_CMD_LEN);
    cmd_len = sprintf(CMD, "%s\r\n", at_cmd);
    GSM_UART_TxBuf(CMD, cmd_len);
}

void gsm_send_test(void)
{
    int ret = -1;
    int len = 0;
    DPRINTF(LOG_INFO, "+++++send gps data++++");
    Gsm_print("AT+QISEND=1,75");
    Gsm_print("$GPGGA,134303.00,3418.040101,N,10855.904676,E,1,07,1.0,418.5,M,-28.0,M,,*4A");
    ret = Gsm_WaitRspOK(NULL, GSM_GENER_CMD_TIMEOUT * 40, true);
    DPRINTF(LOG_DEBUG, " gps_data send ret= %d\r\n", ret);
    DPRINTF(LOG_INFO, "+++++send sensor data++++");
    Gsm_print("AT+QISEND=1,170");
    ret = Gsm_WaitRspOK(NULL, GSM_GENER_CMD_TIMEOUT * 40, true);
}

int Gsm_test_hologram(void)
{
    int ret = -1;
    int time_count;
    int cmd_len;
    int retry_count;

    Gsm_print("AT+COPS=?");
    ret = Gsm_WaitRspOK(NULL, GSM_GENER_CMD_TIMEOUT, true);
    vTaskDelay(300);

    Gsm_print("AT+COPS=1,0,\"CHINA MOBILE\",0");
    ret = Gsm_WaitRspOK(NULL, GSM_GENER_CMD_TIMEOUT, true);
    vTaskDelay(300);

    while((Gsm_CheckNetworkCmd() < 0))
    {
        //delay_ms(GSM_CHECKSIM_RETRY_TIME);
        vTaskDelay(300);
        if(++time_count > GSM_CHECKSIM_RETRY_NUM)
        {
            DPRINTF(LOG_WARN, "check network timeout\r\n");
            return -1;
        }
    }

    Gsm_print("AT+QNWINFO");
    memset(GSM_RSP, 0, GSM_GENER_CMD_LEN);
    ret = Gsm_WaitRspOK(GSM_RSP, GSM_GENER_CMD_TIMEOUT * 4, true);
    if(ret >= 0)
    {
        DPRINTF(LOG_INFO, "Wait +QNWINFO RSP!\n");
        if(NULL != strstr(GSM_RSP, "+QNWINFO: \"EDGE\""))
        {
            ret = 0;
        }
        else
        {
            ret = -1;
        }
    }
    vTaskDelay(300);

    Gsm_print("AT+COPS?");
    memset(GSM_RSP, 0, GSM_GENER_CMD_LEN);
    ret = Gsm_WaitRspOK(GSM_RSP, GSM_GENER_CMD_TIMEOUT, true);
    if(ret >= 0)
    {
        if(NULL != strstr(GSM_RSP, "Hologram"))
        {
            ret = 0;
        }
        else
        {
            ret = -1;
        }
    }
    vTaskDelay(300);

    memset(GSM_RSP, 0, GSM_GENER_CMD_LEN);
    Gsm_print("AT+QICSGP=1,1,\"hologram\",\"\",\"\",1");
    ret = Gsm_WaitRspOK(GSM_RSP, GSM_GENER_CMD_TIMEOUT, true);
    vTaskDelay(300);


    Gsm_print("AT+QIACT=1");
    ret = Gsm_WaitRspOK(NULL, GSM_GENER_CMD_TIMEOUT * 4, true);
    DPRINTF(LOG_INFO, "AT+QIACT=1\n");
    vTaskDelay(300);


    retry_count = 3;
    do
    {
        retry_count--;
        Gsm_print("AT+QIACT?");
        memset(GSM_RSP, 0, GSM_GENER_CMD_LEN);
        ret = Gsm_WaitRspOK(GSM_RSP, GSM_GENER_CMD_TIMEOUT, true);
        if(ret >= 0)
        {
            if(NULL != strstr(GSM_RSP, "+QIACT: 1,1,1"))
            {
                ret = 0;
            }
            else
            {
                ret = -1;
            }
        }
    }
    while(retry_count && ret);
    vTaskDelay(100);

    retry_count = 3;
    do
    {
        retry_count--;
        Gsm_print("AT+QIOPEN=1,0,\"TCP\",\"cloudsocket.hologram.io\",9999,0,1");
        memset(GSM_RSP, 0, GSM_GENER_CMD_LEN);
        ret = Gsm_WaitRspOK(GSM_RSP, GSM_GENER_CMD_TIMEOUT, true);
    }
    while(retry_count && ret);
    vTaskDelay(300);

    retry_count = 2;
    do
    {
        retry_count--;
        Gsm_print("AT+QISEND=0,48");
        ret = Gsm_WaitSendAck(GSM_GENER_CMD_TIMEOUT);
    }
    while(retry_count && ret);
    vTaskDelay(300);
    if(ret == 0)
    {
        DPRINTF(LOG_INFO, "------GSM_SEND_DATA\n");
        Gsm_print("{\"k\":\"+C7pOb8=\",\"d\":\"Hello,World!\",\"t\":\"TOPIC1\"}");
    }

    memset(GSM_RSP, 0, GSM_GENER_CMD_LEN);
    ret = Gsm_WaitRspOK(GSM_RSP, GSM_GENER_CMD_TIMEOUT, true);
    if(ret >= 0)
    {
        if(NULL != strstr(GSM_RSP, "SEND OK"))
        {
            ret = 0;
        }
        else
        {
            ret = -1;
        }
    }
    return ret;
}

//Check Network register Status
int Gsm_CheckNetworkCmd(void)
{
    int ret = -1;
    //
    char *cmd;

    cmd = (char *)malloc(GSM_GENER_CMD_LEN);
    if(cmd)
    {
        uint8_t cmd_len;
        memset(cmd, 0, GSM_GENER_CMD_LEN);
        cmd_len = sprintf(cmd, "%s\r\n", GSM_CHECKNETWORK_CMD_STR);
        //DPRINTF(LOG_INFO, "%s", cmd);
        GSM_UART_TxBuf((uint8_t *)cmd, cmd_len);
        memset(cmd, 0, GSM_GENER_CMD_LEN);
        ret = Gsm_WaitRspOK(cmd, GSM_GENER_CMD_TIMEOUT, true);

        if(ret >= 0)
        {

            if (strstr(cmd, GSM_CHECKNETWORK_RSP_OK))
            {
                ret = 0;
            }
            else if (strstr(cmd, GSM_CHECKNETWORK_RSP_OK_5))
            {
                ret = 0;
            }
            else
            {
                ret = -1;
            }
        }


        free(cmd);
    }
    return ret;
}


void Gsm_CheckAutoBaud(void)
{
    uint8_t  is_auto = true, i = 0;
    uint16_t time_count = 0;
    uint8_t  str_tmp[64];

    delay_ms(800);
    //check is AutoBaud
    memset(str_tmp, 0, 64);

    do
    {
        int       c;
        c = Gsm_RxByte();
        if(c <= 0)
        {
            time_count++;
            delay_ms(2);
            continue;
        }

        //R485_UART_TxBuf((uint8_t *)&c,1);
        if(i < 64)
        {
            str_tmp[i++] = (char)c;
        }

        if (i > 3 && is_auto == true)
        {
            if(strstr((const char*)str_tmp, FIX_BAUD_URC))
            {
                is_auto = false;
                time_count = 800;  //Delay 400ms
            }
        }
    }
    while(time_count < 1000);   //time out 2000ms

    if(is_auto == true)
    {
        Gsm_AutoBaud();

        DPRINTF(LOG_DEBUG, "\r\n  Fix baud\r\n");
        Gsm_FixBaudCmd(GSM_FIX_BAUD);
    }
}



void Gsm_Gpio_Init(void)
{
    nrf_gpio_cfg_output(GSM_PWR_ON_PIN);
    nrf_gpio_cfg_output(GSM_RESET_PIN);
    nrf_gpio_cfg_output(GSM_PWRKEY_PIN);
}

int Gsm_Init()
{
    //int  ret;
    int time_count;
    rak_uart_init(GSM_USE_UART,GSM_RXD_PIN,GSM_TXD_PIN,UART_BAUDRATE_BAUDRATE_Baud115200);
    delay_ms(800);

    Gsm_Gpio_Init();
    Gsm_PowerUp();

    DPRINTF(LOG_INFO, "check auto baud\r\n");
    /*module init ,check is auto baud,if auto,config to 115200 baud.*/
    Gsm_CheckAutoBaud();

    DPRINTF(LOG_INFO, "set echo\r\n");
    /*isable cmd echo*/
    Gsm_SetEchoCmd(0);

    DPRINTF(LOG_INFO, "check sim card\r\n");
    /*check SIM Card status,if not ready,retry 60s,return*/
    time_count = 0;
    while((Gsm_CheckSimCmd() < 0))
    {
        delay_ms(GSM_CHECKSIM_RETRY_TIME);

        if(++time_count > GSM_CHECKSIM_RETRY_NUM)
        {
            DPRINTF(LOG_WARN, "check sim card timeout\r\n");
            return -1;
        }
    }

    return 0;
}
/**
* @}
*/





