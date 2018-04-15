/*************************************************************/
/**                 BQ ZUM JUNIOR - Example                 **/
/**                                                         **/
/** Connections:                                            **/
/**   - Port 1 (D9/A2): LED Yellow                          **/
/**   - Port 2 (D3/A3):                                     **/
/**   - Port 3 (D5/A0): LED Green                           **/
/**   - Port 4 (D6/A1): LED Red                             **/
/**   - Port A (D20/D21/I2C1): 7 Segment Display            **/
/**   - Port B (A4/A5/I2C0): Sensor Multiple                **/
/*************************************************************/
/**  Temperature Displayed in 7 Segment Display             **/
/**  Distance represented in RGB LED                        **/
/**  In low light condition White LEDs are turned ON        **/
/**  Color sensor is represented in RGB LED                 **/
/*************************************************************/
/**  Needed external library: SoftPWM                       **/
/*************************************************************/

#include <BQZUM_DistSensor.h>
#include <BQZUM_TempSensor.h>
#include <BQZUM_ColorSensor.h>
#include <BQZUM_SegmentDisplay.h>
#include <SoftPWM.h>

#define   LED_RGB_RED   7
#define   LED_RGB_GREEN 8
#define   LED_RGB_BLUE  2
#define   BUZZER        4

#define   BLOQ_LED_G_GREEN  A0
#define   BLOQ_LED_G_WHITE  5
#define   BLOQ_LED_Y_YELLOW A2
#define   BLOQ_LED_Y_WHITE  9
#define   BLOQ_LED_R_RED    A1
#define   BLOQ_LED_R_WHITE  6

#define   NOTE_E6       1319
#define   NOTE_B4       494
#define   QUARTER_NOTE  250
#define   EIGHT_NOTE    125

// Declare all sensors
BQZUM_DistSensor      ALPSSensor;
BQZUM_ColorSensor     ColorSensor;
BQZUM_TempSensor      TempSensor;
BQZUM_SegmentDisplay  SegmentDisplay;

void setup() {
  ALPSStat    ALPS_Status = ALPS_OK;
  TempStat    Temp_Status = TEMP_OK;
  ColorStat   Color_Status = COLOR_OK;
  SegmentStat Segment_Status = SEGMENT_OK;
  
  pinMode (LED_RGB_RED,   OUTPUT);
  pinMode (LED_RGB_GREEN, OUTPUT);
  pinMode (LED_RGB_BLUE,  OUTPUT);
  pinMode (BUZZER,        OUTPUT);
  pinMode (BLOQ_LED_G_GREEN, OUTPUT);
  pinMode (BLOQ_LED_G_WHITE, OUTPUT);
  pinMode (BLOQ_LED_R_RED,   OUTPUT);
  pinMode (BLOQ_LED_R_WHITE, OUTPUT);
 
  digitalWrite(LED_RGB_RED,   HIGH);
  digitalWrite(LED_RGB_GREEN, HIGH);
  digitalWrite(LED_RGB_BLUE,  HIGH);
  digitalWrite(BLOQ_LED_G_GREEN, LOW);
  digitalWrite(BLOQ_LED_G_WHITE, LOW);
  digitalWrite(BLOQ_LED_R_RED,   LOW);
  digitalWrite(BLOQ_LED_R_WHITE, LOW);

  Serial.begin(115200);
  Serial.println("-------------------------------------------");
  Serial.println("-    BQ ZUM JUNIOR - SENSORS AND LEDS     -");
  Serial.println("-------------------------------------------");
  Serial.println();

  SoftPWMBegin(SOFTPWM_INVERTED);

  // Can't use tone and SoftPWM at the same time
  //tone(BUZZER, NOTE_E6, QUARTER_NOTE);
  //tone(BUZZER, NOTE_B4, EIGHT_NOTE);
    
  ALPS_Status = ALPSSensor.begin(0);
  if (ALPS_Status == ALPS_OK)
  {
    Serial.println(" - ALPS Sensor running.");
  }
  delay(100);
    
  Color_Status = ColorSensor.begin(0);
  if (Color_Status == COLOR_OK)
  {
    Serial.println(" - Color Sensor running.");
  }
  delay(100);
  
  Temp_Status = TempSensor.begin(0);
  if (Temp_Status == TEMP_OK)
  {
    Serial.println(" - Temperature Sensor running.");
  }
  delay(100);

  Segment_Status = SegmentDisplay.begin(1);
  if (Segment_Status == TEMP_OK)
  {
    Serial.println(" - 7 Segment Display Sensor running.");
  }
  delay(100);

  SegmentDisplay.displayChar('b','q');

  delay(2000);
}

void loop() {
  // put your main code here, to run repeatedly:
  float     f_Red         = 0;
  float     f_RedAvr      = 0;
  float     f_Green       = 0;
  float     f_GreenAvr    = 0;
  float     f_Blue        = 0;
  float     f_BlueAvr     = 0;
  int       i_Red         = 0;
  int       i_Green       = 0;
  int       i_Blue        = 0;
  float     f_Clear       = 0;
  float     f_Temperature = 0;
  uint16_t  ui16_Distance = 0;
  float     f_AmbientLight = 0;
  char      c_Temperature[2];
  
  f_Temperature = TempSensor.getTemp();
  ColorSensor.getColor(&f_Red, &f_Green, &f_Blue, &f_Clear);
  ui16_Distance = ALPSSensor.getDistance();
  f_AmbientLight = ALPSSensor.getAL();

  Serial.print("Temp (ºC): ");
  Serial.print(f_Temperature);
  Serial.print("\tAL (lux): ");
  Serial.print(f_AmbientLight);
  Serial.print("\tDistance: ");
  Serial.println(ui16_Distance);
  Serial.print("COLOR Red:");
  Serial.print(f_Red);
  Serial.print("\tGreen:");
  Serial.print(f_Green);
  Serial.print("\tBlue:");
  Serial.print(f_Blue);
  Serial.print("\tClear:");
  Serial.print(f_Clear);
  Serial.println();
  Serial.println();

  // Distance
  if (ui16_Distance > 1500) {
    digitalWrite(BLOQ_LED_R_RED, LOW);
    digitalWrite(BLOQ_LED_G_GREEN, HIGH);
  }
  else if (ui16_Distance > 400) {
    digitalWrite(BLOQ_LED_R_RED, LOW);
    digitalWrite(BLOQ_LED_G_GREEN, LOW);
  }
  else {
    digitalWrite(BLOQ_LED_R_RED, HIGH);
    digitalWrite(BLOQ_LED_G_GREEN, LOW);
  }

  // Ambient Light
  if (f_AmbientLight < 100) {
    digitalWrite(BLOQ_LED_G_WHITE, HIGH);
    digitalWrite(BLOQ_LED_Y_WHITE, HIGH);
    digitalWrite(BLOQ_LED_R_WHITE, HIGH);
  }
  else {
    digitalWrite(BLOQ_LED_G_WHITE, LOW);
    digitalWrite(BLOQ_LED_Y_WHITE, LOW);
    digitalWrite(BLOQ_LED_R_WHITE, LOW);
  }

  // Color
  f_RedAvr    = 100*f_Red / (f_Red + f_Green + f_Blue);
  f_GreenAvr  = 100*f_Green / (f_Red + f_Green + f_Blue);
  f_BlueAvr   = 100*f_Blue / (f_Red + f_Green + f_Blue);
  i_Red = f_RedAvr;
  i_Green = f_GreenAvr;
  i_Blue = f_BlueAvr;
  SoftPWMSetPercent(LED_RGB_RED, i_Red);
  SoftPWMSetPercent(LED_RGB_GREEN, i_Green);
  SoftPWMSetPercent(LED_RGB_BLUE, i_Blue);

  // Temperature
  dtostrf(f_Temperature,2,0,c_Temperature);
  SegmentDisplay.displayChar(c_Temperature[0],c_Temperature[1]);
  
  delay(500);
}
