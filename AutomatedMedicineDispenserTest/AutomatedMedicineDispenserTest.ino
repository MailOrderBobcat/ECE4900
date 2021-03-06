#include <Time.h>
#include <TimeAlarms.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <SPI.h>       // this is needed for display
#include <Adafruit_ILI9341.h>
#include <Wire.h>      // this is needed for FT6206
#include <Adafruit_FT6206.h>
#include "Keypad.h"
#include "Dispense.h"

// The FT6206 uses hardware I2C (SCL/SDA)
Adafruit_FT6206 ctp = Adafruit_FT6206();

// The display also uses hardware SPI, plus #9 & #10
#define TFT_CS 22
#define TFT_DC 24
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

int pillNum[4] = {0}; // The number of pills to dispense for each pill type

void setCurrentDateTime(Adafruit_ILI9341 tft, Adafruit_FT6206 ctp);
void digitalClockDisplay();
void printDigits(int digits);
void printAlarm(String str);
void setAlarm(Adafruit_ILI9341 tft, Adafruit_FT6206 ctp, int pill);
void alarm1();
void alarm2();
void alarm3();
void alarm4();

Servo servo0; Servo servo1; Servo servo2; Servo servo3;

// Pin for Piezo buzzer
#define PIEZO 13


void setup() {
  //Set up servos
  servo0.attach(9);
  servo0.write(0);
  servo1.attach(10);
  servo1.write(0);
  servo2.attach(11);
  servo2.write(0);
  servo3.attach(12);
  servo3.write(0);

  // Set up piezo buzzer
  pinMode(PIEZO, OUTPUT);
  
  Serial.begin(115200);
  Serial.println(F("Get Current Date"));
  
  tft.begin();
  tft.setRotation(3);

  if(!ctp.begin(40)){  // pass in 'sensitivity' coefficient
    Serial.println("Couldn't start FT6206 touchscreen controller");
    while (1);
  }

  Serial.println("Capacitive touchscreen started");
  
  tft.fillScreen(ILI9341_BLACK);
  setCurrentDateTime(tft, ctp);
  tft.fillScreen(ILI9341_BLACK);
  for( int i = 0; i < sizeof(pillNum)/sizeof(int); i++){
    setAlarm(tft, ctp, i);
  }
}

void loop() {
  digitalClockDisplay();
  Alarm.delay(1000);
}

void setCurrentDateTime(Adafruit_ILI9341 tft, Adafruit_FT6206 ctp){
  /*
  String thingsToGet[5] = {
    "year", "month", "day", "hour", "minute"
  };
  String request = "Please enter the current ";
  int dateTimeData[5] = {2018, 4, 1, 12, 0};
  for(int i = 0; i < sizeof(dateTimeData)/sizeof(int); i++){
    dateTimeData[i] = getNumFromKeypad(tft, ctp, request, thingsToGet[i]);
    Serial.print(thingsToGet[i]); Serial.println(dateTimeData[i]);
  }
  */
    int dateTimeData[5] = {2018, 4, 1, 12, 0}; // hardcode
  // set the current time. This is out of order since setTime takes hours, minutes, seconds, days, months, year
  setTime(dateTimeData[3], dateTimeData[4], 0, dateTimeData[2], dateTimeData[1], dateTimeData[0]);
}

void setAlarm(Adafruit_ILI9341 tft, Adafruit_FT6206 ctp, int pill){
  String pillStr = "Pill " + String(pill);

  /* Hard coding for testing
  int alarmHour = getNumFromKeypad(tft, ctp, "Enter the hour for", pillStr);
  int alarmMinute = getNumFromKeypad(tft, ctp, "Enter the minute for", pillStr);
  pillNum[pill] = getNumFromKeypad(tft, ctp, "How many pills for ", pillStr);
  */
  int alarmHour = 12;
  pillNum[pill] = 3;
  if ( pill == 0 ){
    int alarmMinute = 1;
    Alarm.alarmRepeat(alarmHour, alarmMinute, 0, alarm0);
  } else if( pill == 1){
    int alarmMinute = 1;
    Alarm.alarmRepeat(alarmHour, alarmMinute, 0, alarm1);
  } else if( pill == 2){
    int alarmMinute = 3;
    Alarm.alarmRepeat(alarmHour, alarmMinute, 0, alarm2);
  } else {
    int alarmMinute = 4;
    Alarm.alarmRepeat(alarmHour, alarmMinute, 0, alarm3);
  }
}

// Print the current time to serial
void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.println();
  tft.fillScreen(ILI9341_BLACK);
  tft.setTextSize(3);
  tft.setCursor(0,0);
  tft.print(hour());
  tft.print(":");
  tft.print(minute());
  tft.print(":");
  tft.println(second());
}

void printDigits(int digits){
  Serial.print(":");
  // put a leading zero on numbers under 10
  if(digits < 10){
    Serial.print('0');
  }
  Serial.print(digits);
}

void printAlarm(String str){
  Serial.print("An Alarm for ");
  Serial.print(str);
  Serial.println(" is going off");
}

void alarm0(){
  printAlarm("Pill 0");
  dispense(servo0, pillNum[0], PIEZO);
}
void alarm1(){
  printAlarm("Pill 1");
  dispense(servo1, pillNum[1], PIEZO);
}
void alarm2(){
  printAlarm("Pill 2");
  dispense(servo2, pillNum[2], PIEZO);
}
void alarm3(){
  printAlarm("Pill 3");
  dispense(servo3, pillNum[3], PIEZO);
}

