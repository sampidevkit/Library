#include "libcomp.h"
#include "TaskManager.h"

#define __debug(...) //printf(__VA_ARGS__)

task_t *pCurrTask=NULL; // external variable
uint8_t TaskCount=0; // external variable

public void TaskManager_Add_Task(task_t *pTaskCxt)
{
    if(pTaskCxt->pNext!=NULL) // already in queue
    {
        __debug("\nTask is already in queue\n");
        return;
    }

    pTaskCxt->Lock=0;
    pTaskCxt->pLabel=NULL;

    if(TaskCount>0)
    {
        pTaskCxt->pNext=pCurrTask->pNext; // backup next-context of current task
        pCurrTask->pNext=pTaskCxt;
    }
    else
    {
        pTaskCxt->pNext=pTaskCxt;
        pCurrTask=pTaskCxt;
    }

    TaskCount++;
    __debug("\nTask count %d, new task at %08X\n", TaskCount, pTaskCxt);
}

public void TaskManager_Remove_Task(task_t *pTaskCxt)
{
    uint8_t i=0;

    if(TaskCount==0)
    {
        __debug("\nThere is no any task\n");
        return;
    }

    do
    {
        if(pCurrTask->pNext==pTaskCxt)
        {
            TaskCount--;
            __debug("\nRemove task at %08X. ", pTaskCxt);
            pCurrTask->pNext=pTaskCxt->pNext;
            pTaskCxt->pNext=NULL;
            break;
        }
        else
        {
            i++;
            pCurrTask=pCurrTask->pNext;
        }
    }
    while(i<TaskCount);

    __debug("Task count %d\n", TaskCount);
}

public bool TaskManager_SendMessage(fnc_t pTaskFnc, void *pMsg)
{
    uint8_t i;
    bool rslt=0;

    __debug("\nStart %08X ", pTaskFnc);

    for(i=0; i<TaskCount; i++)
    {
        if(pCurrTask->pFnc==pTaskFnc)
        {
            if(pCurrTask->pMsg==NULL)
            {
                rslt=1;
                pCurrTask->pMsg=pMsg;
            }
        }

        pCurrTask=pCurrTask->pNext;
    }

    __debug("End %08X\n", pTaskFnc);

    return rslt; // 1-done, 0-busy
}

public void TaskManager_Init(uint8_t SysPwrMode)
{
    pCurrTask=NULL;
    TaskCount=0;

    switch(SysPwrMode)
    {
        case SYSPWR_IDLE_MODE:
            set_sleep_mode(0x00); // Idle Sleep Mode enabled
            break;

        case SYSPWR_STANDBY_MODE:
            set_sleep_mode(0x01); // Standby Sleep Mode enabled: WDT, RTC run
            break;

        case SYSPWR_POWER_DOWN_MODE:
            set_sleep_mode(0x02); // Power-Down Sleep Mode enabled: only PIT & WDT run
            break;

        default:
            return;
    }

    sleep_enable();
}