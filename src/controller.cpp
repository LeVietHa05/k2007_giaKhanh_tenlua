// #include <Arduino.h>
// #include <Wire.h>
// #include <Adafruit_GFX.h>
// #include <Adafruit_SH110X.h>
// #include <SPI.h>
// #include <nRF24L01.h>
// #include <RF24.h>
// 
// // Create an RF24 object
// RF24 radio(9, 10); // CE, CSN pins
// #define SSD1306_I2C_ADDRESS 0x3c
// #define SCREEN_WIDTH 128 // OLED display width, in pixels
// #define SCREEN_HEIGHT 64 // OLED display height, in pixels
// 
// // Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// #define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
// Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
// 
// #define ENCODER1_BTN 2
// #define ENCODER1_A 5
// #define ENCODER1_B 4
// 
// #define ENCODER2_BTN 3
// #define ENCODER2_A 7
// #define ENCODER2_B 6
// //--------------------------------------------------
// struct ControlData // 8 bytes 8 channels
// {
//     int ch1;  // 0-1023
//     int ch2;  // 0-1023
//     byte ch3; // 0-1 //fuel pump
//     byte ch4; // 0-1 //ignition
//     byte ch5;
//     byte ch6;
//     byte ch7;
//     byte ch8;
// } controlData;
// //--------------------------------------------------
// int en1pos = 0;
// int en2pos = 0;
// int lastN1 = 0;
// int lastN2 = 0;
// int en1dir = 0;
// int en2dir = 0;
// int n1 = LOW;
// int n2 = LOW;
// 
// void setup()
// {
//     Serial.begin(9600);
// 
//     pinMode(LED_BUILTIN, OUTPUT);
//     pinMode(ENCODER1_BTN, INPUT_PULLUP);
//     pinMode(ENCODER1_A, INPUT_PULLUP);
//     pinMode(ENCODER1_B, INPUT_PULLUP);
//     pinMode(ENCODER2_BTN, INPUT_PULLUP);
//     pinMode(ENCODER2_A, INPUT_PULLUP);
//     pinMode(ENCODER2_B, INPUT_PULLUP);
// 
//     // initialize with the I2C addr 0x3C (for the 128x64)
//     if (!display.begin(SSD1306_I2C_ADDRESS, true))
//     {
//         Serial.println(F("SSD1306 allocation failed"));
//         // for (;;) {
//         //   digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
//         //   delay(1000);
//         // }
//     }
//     Serial.println(F("SSD1306 allocation success"));
// 
//     // Clear the buffer
//     display.clearDisplay();
// 
//     // Display text
//     display.setTextSize(1);             // Normal 1:1 pixel scale
//     display.setTextColor(SH110X_WHITE); // Draw white text
//     display.setCursor(0, 0);            // Start at top-left corner
//     display.println("Controller staring...");
//     // Update the display with the new text
//     display.display();
//     // Initializ;e the nRF24L01 radio
//     if (!radio.begin())
//     {
//         Serial.println(F("NRF failed. Restart the device!"));
//         display.clearDisplay();
//         display.setCursor(0, 0);
//         display.println(F("NRF failed. Restart the device"));
//         display.display();
//         delay(1000);
//         while (1)
//         {
//         } // Hold in infinite loop
//     }
//     // show the radio is working in oled
//     display.setCursor(39, 0);
//     display.println(F("Radio initialized"));
// 
//     radio.setAutoAck(false);
//     // Set the data rate
//     radio.setDataRate(RF24_250KBPS);
//     // Open a writing pipe
//     radio.openWritingPipe(0xf0f0f0f066);
//     // Start listening
//     // radio.startListening();
// }
// 
// void loop()
// {
//     readEncoder();
//     readBtn();
//     // showData();
//     // send data to the rev
// }
// 
// void readBtn()
// {
//     if (digitalRead(ENCODER1_BTN) == LOW)
//     {
//         controlData.ch3 = 1;
//     }
//     else
//     {
//         controlData.ch3 = 0;
//     }
//     if (digitalRead(ENCODER2_BTN) == LOW)
//     {
//         controlData.ch4 = 1;
//     }
//     else
//     {
//         controlData.ch4 = 0;
//     }
// }
// 
// void readEncoder()
// {
//     n1 = digitalRead(ENCODER1_A);
// 
//     if (n1 != lastN1)
//     {
//         if (digitalRead(ENCODER1_B) != n1)
//         {
//             en1pos++;
//             // if (en1pos < 0)
//             //   en1pos = 0;
//         }
//         else
//         {
//             en1pos--;
//             // if (en1pos > 200)  // 200 steps mean 10 rounds or 3600'
//             //   en1pos = 200;    // this also mean the
//         }
//         Serial.print("encoder: \t");
//         Serial.print(en1pos);
//         Serial.print("\t");
//         Serial.println(en2pos);
//         radio.write(&controlData, sizeof(controlData));
//     }
//     controlData.ch1 = en1pos;
//     lastN1 = n1;
// 
//     n2 = digitalRead(ENCODER2_A);
//     if (n2 != lastN2)
//     {
//         if (digitalRead(ENCODER2_B) != n2)
//         {
//             en2pos++;
//             // if (en2pos < 0)
//             //   en2pos = 0;
//         }
//         else
//         {
//             en2pos--;
//             // if (en2pos > 200)  // 200 steps mean 10 rounds or 3600'
//             //   en2pos = 200;    // this also mean the
//         }
//         Serial.print("encoder: \t");
//         Serial.print(en1pos);
//         Serial.print("\t");
//         Serial.println(en2pos);
//         radio.write(&controlData, sizeof(controlData));
//     }
//     controlData.ch2 = en2pos;
//     lastN2 = n2;
// }
// 
// void showData()
// {
//     display.clearDisplay();
// 
//     // Left side for Encoder 1
//     display.setCursor(0, 0);
//     display.println(F("Encoder 1:"));
//     display.print(F("Pos: "));
//     display.println(en1pos);
//     display.print(F("Btn: "));
//     display.println(controlData.ch3 ? F("Pressed") : F("Released"));
// 
//     // Right side for Encoder 2
//     display.setCursor(SCREEN_WIDTH / 2, 0);
//     display.println(F("Encoder 2:"));
//     display.setCursor(SCREEN_WIDTH / 2, 15);
//     display.print(F("Pos: "));
//     display.setCursor(SCREEN_WIDTH / 2, 30);
//     display.println(en2pos);
//     display.setCursor(SCREEN_WIDTH / 2, 45);
//     display.print(F("Btn: "));
//     display.println(controlData.ch4 ? F("Pressed") : F("Released"));
// 
//     display.display();
// }