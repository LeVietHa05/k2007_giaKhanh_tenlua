#include <Arduino.h>
#include <AccelStepper.h>
#include <MultiStepper.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
//==================================================================================================
#define dw digitalWrite
#define dr digitalRead
// stepper motor pins
#define EN1 2
#define DIR1 3
#define EN2 4
#define DIR2 5
#define FULL_ROUND_STEP 800
int pos1 = 0;
int pos2 = 0;
//--------------------------------------
// relay pins
#define PUMPPIN 6
#define VALVEPIN 7
//--------------------------------------
// nrf24 defines &
#define CE 9
#define CSN 10
const uint64_t address = 0xf0f0f0f066;
//--------------------------------------
unsigned long lastTime = 0;
//==================================================================================================
AccelStepper stepper1(AccelStepper::DRIVER, EN1, DIR1);
AccelStepper stepper2(AccelStepper::DRIVER, EN2, DIR2);

MultiStepper steppers;
RF24 radio(CE, CSN);

//==================================================================================================
struct ControlData // 8 bytes 8 channels
{
  int ch1;  // 0-1023
  int ch2;  // 0-1023
  byte ch3; // 0-1
  byte ch4; // 0-1
  byte ch5;
  byte ch6;
  byte ch7;
  byte ch8;
} controlData;
//==================================================================================================
void setup()
{
  Serial.begin(9600);

  //--------------------------------------
  stepper1.setMaxSpeed(4000);
  stepper2.setMaxSpeed(4000);

  steppers.addStepper(stepper1);
  steppers.addStepper(stepper2);

  //--------------------------------------
  pinMode(PUMPPIN, OUTPUT);
  pinMode(VALVEPIN, OUTPUT);
  //--------------------------------------
  if (!radio.begin())
  {
    Serial.println(F("radio hardware is not responding!!"));
    while (1)
    {
      dw(IGNITE_LED_PIN, HIGH);
      delay(500);
      dw(IGNITE_LED_PIN, LOW);
    } // hold in infinite loop
  }
  else
  {
    Serial.println(F("radio hardware is  responding!!"));
  }
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS); // Tốc độ dữ liệu
  // radio.setChannel(100);
  // radio.setPALevel(RF24_PA_LOW);
  radio.openReadingPipe(1, address);
  radio.startListening();

  controlData.ch1 = 1024 / 2;
  controlData.ch2 = 1024 / 2;
  controlData.ch3 = 0;
  controlData.ch4 = 0;
  controlData.ch5 = 0;
  controlData.ch6 = 0;
  controlData.ch7 = 0;
  controlData.ch8 = 0;
  //--------------------------------------
}
//==================================================================================================
void revData()
{
  while (radio.available())
  {
    radio.read(&controlData, sizeof(controlData));
    lastTime = millis();
  }
}
//==================================================================================================
void loop()
{
  revData();
  pos1 = map(controlData.ch1, 0, 1023, 0, FULL_ROUND_STEP);
  pos1 = map(controlData.ch2, 0, 1023, 0, FULL_ROUND_STEP);
  steppers.moveTo(pos1, pos2);
  steppers.setSpeed(1000);
  steppers.runSpeedToPosition(); // non-blocking
  if (controlData.ch3 == 1)
  {
    dw(PUMPPIN, HIGH);
  }
  else
  {
    dw(PUMPPIN, LOW);
  }
}
//==================================================================================================
