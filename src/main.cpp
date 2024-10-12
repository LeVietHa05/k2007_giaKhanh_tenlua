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
#define EN1 3
#define DIR1 2
#define EN2 5
#define DIR2 4
#define FULL_ROUND_STEP 800 // 360 degree
int pos1 = 0;
int pos2 = 0;
int lastPos1 = 0, lastPos2 = 0;
int minAngle1 = 0, maxAngle1 = 360;
int minANgle2 = 10, maxAngle2 = 45;
//--------------------------------------
// button and led and potentiometer
#define IGNITE_LED_PIN 8
// #define FUEL_LED_PIN 23
// #define PITCH_POT_PIN A0 //use in controller
// #define YAW_POT_PIN A1 //use in controller
#define LOWEST_POINT_CHECK_PIN 24
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
bool isPressureFull = false;
//==================================================================================================
AccelStepper stepper1(AccelStepper::DRIVER, EN1, DIR1);
AccelStepper stepper2(AccelStepper::DRIVER, EN2, DIR2);
RF24 radio(CE, CSN);

//==================================================================================================
struct ControlData // 8 bytes 8 channels
{
  int ch1;  // 0-1023
  int ch2;  // 0-1023
  byte ch3; // 0-1 //fuel pump
  byte ch4; // 0-1 //ignition
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
  stepper1.setMaxSpeed(2000);
  stepper1.setAcceleration(1000); // Acceleration
  stepper1.setSpeed(2000);        // Set initial speed

  stepper2.setMaxSpeed(2000);
  stepper2.setAcceleration(1000); // Acceleration
  stepper2.setSpeed(2000);        // Set initial speed

  //--------------------------------------
  pinMode(PUMPPIN, OUTPUT);
  pinMode(VALVEPIN, OUTPUT);
  pinMode(IGNITE_LED_PIN, OUTPUT);
  // pinMode(FUEL_LED_PIN, OUTPUT);
  // pinMode(PITCH_POT_PIN, INPUT);
  // pinMode(YAW_POT_PIN, INPUT);
  pinMode(LOWEST_POINT_CHECK_PIN, INPUT_PULLUP);
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

  controlData.ch1 = FULL_ROUND_STEP / 2; // to the middle
  controlData.ch2 = FULL_ROUND_STEP / 2; // to the middle
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
    Serial.println("radio available");
    radio.read(&controlData, sizeof(controlData));
    lastTime = millis();
  }
}
//==================================================================================================
void checkReachLowestPoint()
{
  // if reach lowest point then stop the stepper
  if (dr(LOWEST_POINT_CHECK_PIN) == LOW)
  {
    stepper2.stop();
  }
}
//==================================================================================================
void loop()
{
  stepper1.run(); // Non-blocking run
  stepper2.run(); // Non-blocking run

  revData();

  pos1 = controlData.ch1 * 20; // gear ratio is 5/1
  pos2 = controlData.ch2;
  pos2 = pos2 * 100;

  stepper1.moveTo(pos1);
  stepper2.moveTo(pos2);
  stepper1.run();
  stepper2.run();

  checkReachLowestPoint();

  // if (controlData.ch3 == 1 && !isPressureFull)
  // {
  //   dw(VALVEPIN, LOW);
  //   dw(PUMPPIN, HIGH);
  //   controlData.ch3 = 0;
  // }
  // else
  // {
  //   dw(VALVEPIN, LOW);
  //   dw(PUMPPIN, LOW);
  // }

  if (controlData.ch4 == 1)
  {
    // count down
    dw(IGNITE_LED_PIN, HIGH);
    delay(1000);
    dw(IGNITE_LED_PIN, LOW);
    delay(1000);
    dw(IGNITE_LED_PIN, HIGH);
    delay(1000);
    dw(IGNITE_LED_PIN, LOW);
    delay(1000);
    dw(IGNITE_LED_PIN, HIGH);
    delay(1000);
    dw(IGNITE_LED_PIN, LOW);

    // launch
    dw(VALVEPIN, HIGH);
    isPressureFull = false;
    controlData.ch4 = 0;
  }
  else
  {
    dw(VALVEPIN, LOW);
  }

  // ready to lauch
  // if (isPressureFull)
  // {
  //   dw(VALVEPIN, LOW);
  //   dw(PUMPPIN, LOW);
  //   dw(FUEL_LED_PIN, HIGH);
  //   delay(1000);
  // }
}
//==================================================================================================
