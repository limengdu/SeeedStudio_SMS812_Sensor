#ifndef _SMS812_H_
#define _SMS812_H_

#include "Arduino.h"






class SMS812_Sensor{
    private:
        Stream *stream;               // e.g. SoftwareSerial or Serial1
        boolean newData;
    public:
        SMS812_Sensor(Stream *s);

};








#endif
