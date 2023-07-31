#ifndef _SMS812_H_
#define _SMS812_H_

#include "Arduino.h"

#define MESSAGE_HEAD 0xAA
#define command_frame_size 6

const unsigned char iomode_frame[6] = {0xAA, 0xAA, 0xF8, 0x01, 0x00, 0x4D};
const unsigned char uartmode_frame[6] = {0xAA, 0xAA, 0xFE, 0x01, 0x00, 0x53};
const unsigned char uartreqmode_frame[6] = {0xAA, 0xAA, 0xFA, 0x01, 0x00, 0x4F};
const unsigned char checkreqmode_frame[6] = {0xAA, 0xAA, 0xFC, 0x01, 0x00, 0x51};

class SMS812_Sensor{
    private:
        Stream *stream;               // e.g. SoftwareSerial or Serial1
        boolean newData;
        byte commandCode, dataLen, checkSum;
        // const unsigned char commandFrame[6];
        // const unsigned char dataFrame[9];
        byte* dataMsg = NULL;
        byte* full_frame = NULL;
    public:
        SMS812_Sensor(Stream *s);
        // void recvRadarBytes();
        // void showData();
        void setIOMode();
        void setUARTMode();
        void setUARTREQMode();
        void checkUARTREQ(int delaytime = 0);
        uint8_t calculate_checksum(const uint8_t* buf, uint8_t size);
        void getFrame();
};








#endif
