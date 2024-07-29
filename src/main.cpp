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
//--------------------------------------
// relay pins
#define PUMPPIN 6
#define VALVEPIN 7
//--------------------------------------
// potentiometer pins
#define STEPPER1_CONTROL_PIN A0
#define STEPPER2_CONTROL_PIN A1
//--------------------------------------
// button pins
// TODO:  change the pin
#define IGNITE_IN_PIN 14  // the button
#define IGNITE_LED_PIN 15 // led of the button
//--------------------------------------
// nrf24 defines &
#define CE 9
#define CSN 10
const uint64_t address = 0xf0f0f0f066;
//==================================================================================================
AccelStepper stepper1(AccelStepper::DRIVER, EN1, DIR1);
AccelStepper stepper2(AccelStepper::DRIVER, EN2, DIR2);

MultiStepper steppers;
RF24 radio(CE, CSN);

//==================================================================================================
struct ControlData // 8 bytes 8 channels
{
  byte ch1;
  byte ch2;
  byte ch3;
  byte ch4;
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
  pinMode(IGNITE_IN_PIN, INPUT_PULLUP);
  pinMode(IGNITE_LED_PIN, OUTPUT);
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
  radio.setAutoAck(true);
  radio.setDataRate(RF24_1MBPS); // Tốc độ dữ liệu
  radio.setChannel(100);
  radio.setPALevel(RF24_PA_LOW);
  radio.openReadingPipe(1, address);
  radio.startListening();
}
//==================================================================================================
void loop()
{
  if (radio.available())
  {
    radio.read(&controlData, sizeof(controlData));
  }
}
//==================================================================================================
