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

void SMS812_Sensor::checkUARTREQ(int delaytime, bool showSwitch){
  stream->write(checkreqmode_frame, command_frame_size);
  stream->flush();
  getFrame(showSwitch);
  material = distance = strength = 0;
  material = this->full_frame[4];
  distance = this->full_frame[5] + this->full_frame[6] << 8;
  strength = this->full_frame[6];
  delay(delaytime);
}

uint8_t SMS812_Sensor::calculate_checksum(const uint8_t* buf, uint8_t size){
  uint32_t sum = 0;
  for(int i = 0; i < size; i++) {
    sum += buf[i];
  }
  return sum & 0xFF;
}


bool SMS812_Sensor::getFrame(bool showSwitch){
  // 等待包头
  if (!stream->available()) {
    return false;
  }
  // 读取包头
  uint8_t frame_head[2];
  stream->readBytes(frame_head, 2);
  // 检查包头是否正确
  if (frame_head[0] != MESSAGE_HEAD || frame_head[1] != MESSAGE_HEAD) {
    return false;
  }
#ifdef DEBUG
  Serial.print("Frame head1: "); Serial.println(frame_head[0], HEX);
  Serial.print("Frame head2: "); Serial.println(frame_head[1], HEX);
#endif

  // 读取命令帧和数据长度帧
  this->commandCode = stream->read();
  this->dataLen = stream->read();
#ifdef DEBUG
  Serial.print("Command code: "); Serial.println(this->commandCode, HEX);
  Serial.print("Data length: "); Serial.println(this->dataLen, HEX);
#endif
  if(this->dataLen > 5) return false;

  // Allocate memory for dataMsg
  if (dataMsg != NULL) {
    free(dataMsg);
    dataMsg = NULL;
  }
  dataMsg = (byte*)malloc(this->dataLen);
  // 读取数据帧
  stream->readBytes(dataMsg, this->dataLen);
#ifdef DEBUG
  Serial.print("Data: ");
  for(int i = 0; i < this->dataLen; i++){
    Serial.print(dataMsg[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
#endif

  // 读取校验帧   
  this->checkSum = stream->read();
#ifdef DEBUG
  Serial.print("Recive checksum: "); Serial.println(this->checkSum);
#endif

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
#ifdef DEBUG
  Serial.print("Calculating checksum: "); Serial.println(cal_checksum);
#endif
  // 检查校验和是否正确
  if (cal_checksum != this->checkSum) {
#ifdef DEBUG
    Serial.println("checksum mismatch");
#endif
    return false;
  }

  // 打印接收到的数据帧
  if(showSwitch) {
    Serial.print("Received frame: ");
    char charVal[4];
    for (int i = 0; i < this->dataLen + 5; i++) {
      sprintf(charVal, "%02X", full_frame[i]);
      Serial.print(charVal);
      Serial.print(" ");
    }
    Serial.println();
  }
  return true;
}

bool SMS812_Sensor::parseDatagram(bool showSwitch) {
  if((!getFrame(showSwitch) && this->dataLen != 4)){
    // Serial.println("Please check whether it is currently UART mode or UART REQ mode!");
    return false;
  }
  material = distance = strength = 0;
  material = this->full_frame[4];
  distance = this->full_frame[5] | (this->full_frame[6] << 8);
  strength = this->full_frame[7];
  return true;
}

bool SMS812_Sensor::parseIO(){
  if (!stream->available()) {
    return false;
  }
  else{
    iomaterial = "Non-blanket";
    return true;
  }
  // byte iodata = stream->read();
  // if(iodata == '\0'){
  //   iomaterial = "Non-blanket";
  //   return true;
  // }
  // else if(iodata == '1'){
  //   iomaterial = "Blanket";
  //   return true;
  // }
  // else
  //   Serial.print(iodata);
  //   return false;
}






