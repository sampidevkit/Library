#ifndef AP33772_H
#define AP33772_H

#include "Common/LibDef.h"
#include "AP33772_Cfg.h"

#define CMD_SRCPDO 0x00
#define CMD_PDONUM 0x1C
#define CMD_STATUS 0x1D
#define CMD_MASK 0x1E
#define CMD_VOLTAGE 0x20
#define CMD_CURRENT 0x21
#define CMD_TEMP 0x22
#define CMD_OCPTHR 0x23
#define CMD_OTPTHR 0x24
#define CMD_DRTHR 0x25
#define CMD_RDO 0x30

#define AP33772_ADDRESS 0x51
#define READ_BUFF_LENGTH     30
#define WRITE_BUFF_LENGTH    6
#define SRCPDO_LENGTH        28

typedef enum {
    READY_EN = 1 << 0, // 0000 0001
    SUCCESS_EN = 1 << 1, // 0000 0010
    NEWPDO_EN = 1 << 2, // 0000 0100
    OVP_EN = 1 << 4, // 0001 0000
    OCP_EN = 1 << 5, // 0010 0000
    OTP_EN = 1 << 6, // 0100 0000
    DR_EN = 1 << 7 // 1000 0000
} AP33772_MASK;

typedef struct {

    union {

        struct {
            uint8_t isReady : 1;
            uint8_t isSuccess : 1;
            uint8_t isNewpdo : 1;
            uint8_t reserved : 1;
            uint8_t isOvp : 1;
            uint8_t isOcp : 1;
            uint8_t isOtp : 1;
            uint8_t isDr : 1;
        };
        uint8_t readStatus;
    };
    uint8_t readVolt; // LSB: 80mV
    uint8_t readCurr; // LSB: 24mA
    uint8_t readTemp; // unit: 1C
} AP33772_STATUS_T;

typedef struct {

    union {

        struct {
            uint8_t newNegoSuccess : 1;
            uint8_t newNegoFail : 1;
            uint8_t negoSuccess : 1;
            uint8_t negoFail : 1;
            uint8_t reserved_1 : 4;
        };
        uint8_t negoEvent;
    };

    union {

        struct {
            uint8_t ovp : 1;
            uint8_t ocp : 1;
            uint8_t otp : 1;
            uint8_t dr : 1;
            uint8_t reserved_2 : 4;
        };
        uint8_t protectEvent;
    };
} EVENT_FLAG_T;

typedef struct {

    union {

        struct {
            unsigned int maxCurrent : 10; // unit: 10mA
            unsigned int voltage : 10; // unit: 50mV
            unsigned int reserved_1 : 10;
            unsigned int type : 2;
        } fixed;

        struct {
            unsigned int maxCurrent : 7; // unit: 50mA
            unsigned int reserved_1 : 1;
            unsigned int minVoltage : 8; // unit: 100mV
            unsigned int reserved_2 : 1;
            unsigned int maxVoltage : 8; // unit: 100mV
            unsigned int reserved_3 : 3;
            unsigned int apdo : 2;
            unsigned int type : 2;
        } pps;

        struct {
            uint8_t byte0;
            uint8_t byte1;
            uint8_t byte2;
            uint8_t byte3;
        };
        unsigned long data;
    };
} PDO_DATA_T;

typedef struct {

    union {

        struct {
            unsigned int maxCurrent : 10; // unit: 10mA
            unsigned int opCurrent : 10; // unit: 10mA
            unsigned int reserved_1 : 8;
            unsigned int objPosition : 3;
            unsigned int reserved_2 : 1;
        } fixed;

        struct {
            unsigned int opCurrent : 7; // unit: 50mA
            unsigned int reserved_1 : 2;
            unsigned int voltage : 11; // unit: 20mV
            unsigned int reserved_2 : 8;
            unsigned int objPosition : 3;
            unsigned int reserved_3 : 1;
        } pps;

        struct {
            uint8_t byte0;
            uint8_t byte1;
            uint8_t byte2;
            uint8_t byte3;
        };
        unsigned long data;
    };
} RDO_DATA_T;

class AP33772 {
public:
    AP33772(TwoWire &wire = Wire);
    void begin();
    void setVoltage(int targetVoltage); // Unit in mV
    void setMaxCurrent(int targetMaxCurrent); // Unit in mA
    void setNTC(int TR25, int TR50, int TR75, int TR100);
    void setDeratingTemp(int temperature);
    void setMask(AP33772_MASK flag, bool state);
    void writeRDO();
    int readVoltage();
    int readCurrent();
    int readTemp();
    void printPDO();
    void reset();

};

#endif
