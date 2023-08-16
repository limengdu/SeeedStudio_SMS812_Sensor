#include "sms812.h"

//#include <SoftwareSerial.h>
// Choose any two pins that can be used with SoftwareSerial to RX & TX
//#define RX_Pin A2
//#define TX_Pin A3

//SoftwareSerial mySerial = SoftwareSerial(RX_Pin, TX_Pin);

// we'll be using software serial
//SMS812_Sensor radar = SMS812_Sensor(&mySerial);

// can also try hardware serial with
SMS812_Sensor radar = SMS812_Sensor(&Serial1);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial1.begin(115200);
  pinMode(A0, INPUT);

  //  mySerial.begin(115200);

  while(!Serial);   //When the serial port is opened, the program starts to execute.

  Serial.println("Ready");

  radar.setUARTREQMode();
}

void loop() {
  // put your main code here, to run repeatedly:
  radar.checkUARTREQ(1000, true);      // Check radar information every second. And turn on raw data frame display.

  // Parses the contents of the data frame. If the function parseDatagram is given a true argument, the raw data frame display is enabled.
  if(radar.material == 0x00){
    Serial.println("No blankets detected.");
    Serial.print("The measured distance is: ");
    Serial.print(radar.distance);
    Serial.println(" mm");
    Serial.print("The ultrasonic signal strength is: ");
    Serial.println(radar.strength);
  }
}
