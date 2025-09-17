#include "at_cmd.h"
#include "misc/util.h"

int8_t ATCMD_ReportOn(void)
{
    return ATCMD_SendGetAck("AT+CMEE=2\r", ATCMD_GetResConst(RES_OK), NULL, 500, 10, 3);
}

int8_t ATCMD_NoFlowCtrl(void)
{
    return ATCMD_SendGetAck("AT&K0\r", ATCMD_GetResConst(RES_OK), NULL, 500, 10, 3);
}

int8_t ATCMD_SetAirplaneMode(void)
{
    return ATCMD_SendGetAck("AT+CFUN=4\r", ATCMD_GetResConst(RES_OK), NULL, 5000, 10, 3);
}

int8_t ATCMD_SetFullFuncMode(void)
{
    return ATCMD_SendGetAck("AT+CFUN=1\r", ATCMD_GetResConst(RES_OK), NULL, 5000, 10, 3);
}

int8_t ATCMD_SetLteOnly(void)
{
    return ATCMD_SendGetAck("AT+WS46=28\r", ATCMD_GetResConst(RES_OK), NULL, 5000, 10, 3);
}

int8_t ATCMD_SetNbPriority(void)
{
    return ATCMD_SendGetAck("AT#WS46=1\r", ATCMD_GetResConst(RES_OK), NULL, 5000, 10, 3);
}

int8_t ATCMD_Reboot(void)
{
    return ATCMD_SendGetAck("AT#REBOOT\r", ATCMD_GetResConst(RES_OK), NULL, 3000, 10, 3);
}

int8_t ATCMD_SysHalt(void)
{
    return ATCMD_SendGetAck("AT#SYSHALT\r", ATCMD_GetResConst(RES_OK), NULL, 5000, 10, 3);
}

int8_t ATCMD_CheckSim(void)
{
    return ATCMD_SendGetAck("AT+CPIN?\r", ATCMD_GetResConst(RES_READY), NULL, 500, 10, 3);
}

int8_t ATCMD_CheckNetReg(uint8_t retry)
{
    int8_t rslt=ATCMD_SendGetAck("AT+CEREG?\r", ",1", ",5", 500, 10, retry);

    if((rslt==RESULT_ACK)||(rslt==RESULT_NACK))
        rslt=RESULT_DONE;

    return rslt;
}

int8_t ATCMD_GetGnssPwrStt(bool *pStt)
{
    int8_t rslt=ATCMD_SendGetAck("AT$GPSP?\r", ": 1", ": 0", 500, 10, 3);

    if(rslt==RESULT_ACK)
    {
        *pStt=1;
        rslt=RESULT_DONE;
    }
    else if(rslt==RESULT_NACK)
    {
        *pStt=0;
        rslt=RESULT_DONE;
    }

    return rslt;
}

int8_t ATCMD_SetGnssPwrStt(bool Stt)
{
    int8_t rslt;

    if(Stt==1)
        rslt=ATCMD_SendGetAck("AT$GPSP=1\r", ATCMD_GetResConst(RES_OK), NULL, 500, 10, 3);
    else
        rslt=ATCMD_SendGetAck("AT$GPSP=0\r", ATCMD_GetResConst(RES_OK), NULL, 500, 10, 3);

    return rslt;
}

int8_t ATCMD_GetImei(char *pStr)
{
    int8_t rslt=ATCMD_SendGetAck("AT#CGSN\r", ATCMD_GetResConst(RES_OK), NULL, 500, 10, 3);

    if(rslt==RESULT_DONE)
        str_sub_between_2sub(pStr, (const char *) ATCMD_GetRxBuffer(0), "#CGSN: ", "\r");

    return rslt;
}

int8_t ATCMD_GetIccId(char *pStr)
{
    int8_t rslt=ATCMD_SendGetAck("AT#CCID\r", ATCMD_GetResConst(RES_OK), NULL, 500, 10, 3);

    if(rslt==RESULT_DONE)
        str_sub_between_2sub(pStr, (const char *) ATCMD_GetRxBuffer(0), "#CCID: ", "\r");

    return rslt;
}

int8_t ATCMD_GetImsi(char *pStr)
{
    int8_t rslt=ATCMD_SendGetAck("AT#IMSI\r", ATCMD_GetResConst(RES_OK), NULL, 500, 10, 3);

    if(rslt==RESULT_DONE)
        str_sub_between_2sub(pStr, (const char *) ATCMD_GetRxBuffer(0), "#IMSI: ", "\r");

    return rslt;
}
