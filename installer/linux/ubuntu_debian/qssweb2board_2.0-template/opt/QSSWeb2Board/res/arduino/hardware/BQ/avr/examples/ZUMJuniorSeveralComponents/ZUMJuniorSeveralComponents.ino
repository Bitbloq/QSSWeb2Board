#include <BQZUMI2C7SegmentDisplay.h>
#include <BQZUMJunior.h>
#include <BQZUMJuniorPorts.h>

BQ::ZUMJunior zumJunior;
const uint8_t slider1Pin = zumJunior.ports[1][0];
const uint8_t slider2Pin = zumJunior.ports[1][1];
const uint8_t led1Pin = zumJunior.ports[2][0];
const uint8_t led2Pin = zumJunior.ports[2][1];
const uint8_t buttonPin = zumJunior.ports[A][1];

const uint8_t segmentsI2CPort = zumJunior.i2cPorts[A];
BQ::ZUM::I2C7SegmentDisplay segmentDisplay(segmentsI2CPort);


void setup() {
  zumJunior.setup();
  segmentDisplay.setup();
  segmentDisplay.displayChar(' ',' ');

  pinMode(slider1Pin,INPUT);
  pinMode(slider2Pin,INPUT);
  pinMode(led1Pin,OUTPUT);
  pinMode(led2Pin,OUTPUT);
  pinMode(buttonPin,INPUT);

  //tone for program starting
  zumJunior.playTone(800, 100);

  //yellow
  zumJunior.setRGBLED(255, 255, 0);
  delay(1000);

  //purple
  zumJunior.setRGBLED(255, 0, 255);
  delay(1000);

  //cyan
  zumJunior.setRGBLED(0, 255, 255);
  delay(1000);
}

void loop() {
  // put your main code here, to run repeatedly:
  uint8_t sliderRead = digitalRead(slider1Pin) + 2*digitalRead(slider2Pin);

  switch(sliderRead){
    case 0:
      segmentDisplay.displayChar('0','0');
      digitalWrite(led1Pin,LOW);
      digitalWrite(led2Pin,LOW);
      break;
    case 1:
      segmentDisplay.displayChar('1','0');
      digitalWrite(led1Pin,HIGH);
      digitalWrite(led2Pin,LOW);
      break;
    case 2:
      segmentDisplay.displayChar('0','1');
      digitalWrite(led1Pin,LOW);
      digitalWrite(led2Pin,HIGH);
      break;
    case 3:
      segmentDisplay.displayChar('1','1');
      digitalWrite(led1Pin,HIGH);
      digitalWrite(led2Pin,HIGH);
      break;
  }

  while(digitalRead(buttonPin) == 1){
    segmentDisplay.displayChar('8','8');
      digitalWrite(led1Pin,HIGH);
      digitalWrite(led2Pin,LOW);
      delay(500);
      digitalWrite(led1Pin,LOW);
      digitalWrite(led2Pin,HIGH);
      delay(500);
  }

}
