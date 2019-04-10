#include "stdint.h"
#include "string.h"
#include "stdbool.h"
#include "stddef.h"
#include "rui.h"


//inner driver function stru 
typedef struct rui_inner_function_stru
{
    uint8_t  driver_name[30];                                //driver name string
    bool     (*start)(void);                                 //driver begin work
    bool     (*suspend)(void);                               //driver suspend work
    bool     (*resume)(void);                                //driver resume work
    bool     (*stop)(void);                                  //driver stop work
    bool     (*mode_switch)(DRIVER_MODE mode);               //driver mode switch
    bool     (*get)((void *)data);                           //get data from sensor like gps, light strength
    bool     (*accuracy_set)((void *)data);                  //set accuracy for sensor  
    bool     (*threshold_set)((void *)data);                 //set threshold for sensor
    bool     (*get_device_id)((void *)data);                 //get device info
    bool     (*reset)(void);                                 //reset device
    bool     (*send)((void *)data);                          //communicate for nb-iot or lora
    void     (*interrupt_call_back1)(void);                  //interrupt callback1 for driver
    void     (*interrupt_call_back2)(void);                  //interrupt callback2 for driver
}rui_inner_function_stru;

//add driver struct sample here,each driver includes a sample
rui_inner_function_stru rui_inner_function_stru[RUI_DRIVER_NUM]={

};



bool rui_start(uint8_t *device)
{
    uint8_t driver_num = 0;
    bool ret = false;
    if (device == NULL)
    {
        return ret;
    }
    for (int driver_num = 0; driver_num < RUI_DRIVER_NUM; driver_num++)
    {
        if (!strcmp(rui_inner_function_stru[driver_num].driver_name,device))
        {
            if (rui_inner_function_stru[driver_num].start != NULL)
            {
                ret = rui_inner_function_stru[driver_num].start();
                if (ret == false)
                {
                    //print log and exit
                    return ret;
                }
                break;
            }

        }
    }
    return ret;
}

bool rui_stop(uint8_t *device)
{
    uint8_t driver_num = 0;
    bool ret = false;
    if (device == NULL)
    {
        return ret;
    }
    for (int driver_num = 0; driver_num < RUI_DRIVER_NUM; driver_num++)
    {
        if (!strcmp(rui_inner_function_stru[driver_num].driver_name,device))
        {
            if (rui_inner_function_stru[driver_num].stop != NULL)
            {
                ret = rui_inner_function_stru[driver_num].stop();
                if (ret == false)
                {
                    //print log and exit
                    return ret;
                }
                break;
            }
        }
    }
    return ret;
}

bool rui_suspend(uint8_t *device)
{
    uint8_t driver_num = 0;
    bool ret = false;
    if (device == NULL)
    {
        return ret;
    }
    for (int driver_num = 0; driver_num < RUI_DRIVER_NUM; driver_num++)
    {
        if (!strcmp(rui_inner_function_stru[driver_num].driver_name,device))
        {
            if (rui_inner_function_stru[driver_num].suspend != NULL)
            {
                ret = rui_inner_function_stru[driver_num].suspend();
                if (ret == false)
                {
                    //print log and exit
                    return ret;
                }
                break;
            }
        }
    }
    return ret;
}

bool rui_resume(uint8_t *device)
{
    uint8_t driver_num = 0;
    bool ret = false;
    if (device == NULL)
    {
        return ret;
    }
    for (int driver_num = 0; driver_num < RUI_DRIVER_NUM; driver_num++)
    {
        if (!strcmp(rui_inner_function_stru[driver_num].driver_name,device))
        {
            if (rui_inner_function_stru[driver_num].resume != NULL)
            {
                ret = rui_inner_function_stru[driver_num].resume();
                if (ret == false)
                {
                    //print log and exit
                    return ret;
                }
                break;
            }
        }
    }
    return ret;
}

bool rui_mode_switch(uint8_t *device,DRIVER_MODE mode)
{
    uint8_t driver_num = 0;
    bool ret = false;
    if (device == NULL)
    {
        return ret;
    }
    for (int driver_num = 0; driver_num < RUI_DRIVER_NUM; driver_num++)
    {
        if (!strcmp(rui_inner_function_stru[driver_num].driver_name,device))
        {
            if (rui_inner_function_stru[driver_num].mode_switch != NULL)
            {
                ret = rui_inner_function_stru[driver_num].mode_switch(mode);
                if (ret == false)
                {
                    //print log and exit
                    return ret;
                }
                break;
            }
        }
    }
    return ret;
}

bool rui_get(uint8_t *device,(void *) data)
{
    uint8_t driver_num = 0;
    bool ret = false;
    if (device == NULL || data == NULL)
    {
        return ret;
    }
    for (int driver_num = 0; driver_num < RUI_DRIVER_NUM; driver_num++)
    {
        if (!strcmp(rui_inner_function_stru[driver_num].driver_name,device))
        {
            if (rui_inner_function_stru[driver_num].get != NULL)
            {
                ret = rui_inner_function_stru[driver_num].get(data);
                if (ret == false)
                {
                    //print log and exit
                    return ret;
                }
                break;
            }
        }
    }
    return ret;
}
bool rui_accuracy_set(uint8_t *device,(void *) data)
{
    uint8_t driver_num = 0;
    bool ret = false;
    if (device == NULL || data == NULL)
    {
        return ret;
    }
    for (int driver_num = 0; driver_num < RUI_DRIVER_NUM; driver_num++)
    {
        if (!strcmp(rui_inner_function_stru[driver_num].driver_name,device))
        {
            if (rui_inner_function_stru[driver_num].accuracy_set != NULL)
            {
                ret = rui_inner_function_stru[driver_num].accuracy_set(data);
                if (ret == false)
                {
                    //print log and exit
                    return ret;
                }
                break;
            }
        }
    }
    return ret;
}

bool rui_threshold_set(uint8_t *device,(void *) data)
{
    uint8_t driver_num = 0;
    bool ret = false;
    if (device == NULL || data == NULL)
    {
        return ret;
    }
    for (int driver_num = 0; driver_num < RUI_DRIVER_NUM; driver_num++)
    {
        if (!strcmp(rui_inner_function_stru[driver_num].driver_name,device))
        {
            if (rui_inner_function_stru[driver_num].threshold_set != NULL)
            {
                ret = rui_inner_function_stru[driver_num].threshold_set(data);
                if (ret == false)
                {
                    //print log and exit
                    return ret;
                }
                break;
            }
        }
    }
    return ret;
}

bool rui_get_device_id(uint8_t *device,(void *) data)
{
    uint8_t driver_num = 0;
    bool ret = false;
    if (device == NULL || data == NULL)
    {
        return ret;
    }
    for (int driver_num = 0; driver_num < RUI_DRIVER_NUM; driver_num++)
    {
        if (!strcmp(rui_inner_function_stru[driver_num].driver_name,device))
        {
            if (rui_inner_function_stru[driver_num].get_device_id != NULL)
            {
                ret = rui_inner_function_stru[driver_num].get_device_id(data);
                if (ret == false)
                {
                    //print log and exit
                    return ret;
                }
                break;
            }
        }
    }
    return ret;
}

bool rui_reset(uint8_t *device,(void *) data)
{
    uint8_t driver_num = 0;
    bool ret = false;
    if (device == NULL || data == NULL)
    {
        return ret;
    }
    for (int driver_num = 0; driver_num < RUI_DRIVER_NUM; driver_num++)
    {
        if (!strcmp(rui_inner_function_stru[driver_num].driver_name,device))
        {
            if (rui_inner_function_stru[driver_num].reset != NULL)
            {
                ret = rui_inner_function_stru[driver_num].reset(data);
                if (ret == false)
                {
                    //print log and exit
                    return ret;
                }
                break;
            }
        }
    }
    return ret;
}

bool rui_send(uint8_t *device,(void *) data)
{
    uint8_t driver_num = 0;
    bool ret = false;
    if (device == NULL || data == NULL)
    {
        return ret;
    }
    for (int driver_num = 0; driver_num < RUI_DRIVER_NUM; driver_num++)
    {
        if (!strcmp(rui_inner_function_stru[driver_num].driver_name,device))
        {
            if (rui_inner_function_stru[driver_num].send != NULL)
            {
                ret = rui_inner_function_stru[driver_num].send(data);
                if (ret == false)
                {
                    //print log and exit
                    return ret;
                }
                break;
            }
        }
    }
    return ret;
}

void rui_interrupt_register(uint8_t *device,interrupt_callback callback)
{
    if (device == NULL || callback == NULL)
    {
        return;
    }
    for (int driver_num = 0; driver_num < RUI_DRIVER_NUM; driver_num++)
    {
        if (!strcmp(rui_inner_function_stru[driver_num].driver_name,device))
        {
            if (rui_inner_function_stru[driver_num].interrupt_call_back1 == NULL)
            {
                rui_inner_function_stru[driver_num].interrupt_call_back1 = callback;
                break;
            }
            else if (rui_inner_function_stru[driver_num].interrupt_call_back2 == NULL)
            {
                rui_inner_function_stru[driver_num].interrupt_call_back2 = callback;
                break;
            }
            else
            {
                return;
            }
        }
    }
}
void rui_function_init()
{
    memset(&rui_function,0,sizeof(rui_function));
    memcpy(rui_function.driver_name,"rui_common_stru",sizeof("rui_common_stru"));
    rui_function.start = rui_start;
    rui_function.stop = rui_stop;
    rui_function.suspend = rui_suspend;
    rui_function.resume = rui_resume;    
    rui_function.mode_switch = rui_mode_switch;
    rui_function.get = rui_get;
    rui_function.accuracy_set = rui_accuracy_set;
    rui_function.get_device_id = rui_get_device_id;
    rui_function.reset = rui_reset;
    rui_function.send = rui_send;
}