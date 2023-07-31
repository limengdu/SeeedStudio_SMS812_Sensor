#include "sms812.h"

SMS812_Sensor::SMS812_Sensor(Stream *s)
    : stream(s){
  this->newData = false;
}

void SMS812_Sensor::setIOMode(){
  stream->write(iomode_frame, command_frame_size);
  stream->flush();
  Serial.println("Set Radar to IO Mode!");
}

void SMS812_Sensor::setUARTMode(){
  stream->write(uartmode_frame, command_frame_size);
  stream->flush();
  Serial.println("Set Radar to UART Mode!");
}

void SMS812_Sensor::setUARTREQMode(){
  stream->write(uartreqmode_frame, command_frame_size);
  stream->flush();
  Serial.println("Set Radar to UART REQ Mode!");
}

void SMS812_Sensor::checkUARTREQ(int delaytime){
  stream->write(checkreqmode_frame, command_frame_size);
  stream->flush();
  delay(delaytime);
  // getFrame();
}

uint8_t SMS812_Sensor::calculate_checksum(const uint8_t* buf, uint8_t size){
  uint32_t sum = 0;
  for(int i = 0; i < size; i++) {
    sum += buf[i];
  }
  // Serial.println(sum, HEX);
  // Serial.println(sum & 0xFF, HEX);
  return sum & 0xFF;
}

// void SMS812_Sensor::getCommandFrame(){  // 获得命令报文，长度6
  // while(stream->available());
  // stream->readBytes(dataFrame)
// }

// void SMS812_Sensor::getDataFrame(){     // 获得数据报文，长度9
  // while(stream->available());

// }


void SMS812_Sensor::getFrame(){
  // 等待包头
  while (stream->available()) {
    // 读取包头
    uint8_t frame_head[2];
    stream->readBytes(frame_head, 2);
    // 检查包头是否正确
    if (frame_head[0] != MESSAGE_HEAD || frame_head[1] != MESSAGE_HEAD) {
      return;
    }

    // 读取命令帧和数据长度帧
    this->commandCode = stream->read();
    this->dataLen = stream->read();

    // Allocate memory for dataMsg
    if (dataMsg != NULL) {
        free(dataMsg);
        dataMsg = NULL;
    }
    dataMsg = (byte*)malloc(this->dataLen);
    // 读取数据帧
    stream->readBytes(dataMsg, this->dataLen);

    // 读取校验帧   
    this->checkSum = stream->read();

    // Allocate memory for dataMsg
    if (full_frame != NULL) {
        free(full_frame);
        full_frame = NULL;
    }
    full_frame = (byte*)malloc(this->dataLen + 5);
    full_frame[0] = MESSAGE_HEAD;
    full_frame[1] = MESSAGE_HEAD;
    full_frame[2] = this->commandCode;
    full_frame[3] = this->dataLen;
    for(int i = 0, j = 4; i < this->dataLen; i++, j++){
      full_frame[j] = this->dataMsg[i];
    }
    full_frame[this->dataLen + 4] = this->checkSum;

    uint8_t cal_checksum = calculate_checksum(full_frame, this->dataLen + 4);

    // 检查校验和是否正确
    if (cal_checksum != this->checkSum) {
      Serial.println("checksum mismatch");
      // return;
    }

    // 打印接收到的数据帧
    Serial.print("Received frame: ");
    char charVal[4];
    for (int i = 0; i < this->dataLen + 5; i++) {
      sprintf(charVal, "%02X", full_frame[i]);
      Serial.print(charVal);
      Serial.print(" ");
    }
    Serial.println();
    return;
  }
}








