//DS1307 library from https://github.com/adafruit/RTClib
//ShiftOut for 74HC595 http://arduino.cc/en/Reference/ShiftOut
//ShiftOut Example http://arduino.cc/en/Tutorial/ShiftOut
//Sundata https://github.com/DataPanik/Sundata

#include "Wire.h"
#include "RTClib.h"
#include "sundata.h"
float loop_sunrise;
float loop_sunset;
float loop_daytime;
float loop_currenttime;
float loop_daylength;
float loop_timelag;
int loop_leds = 16;
int led_current;
int loop_pin_latch = 8;
int loop_pin_clock = 12;
int loop_pin_data = 11;

RTC_DS1307 rtc;
sundata solarloop = sundata(31.2, 121.2, 8);

void setup() {
  pinMode(loop_pin_latch, OUTPUT);
  pinMode(loop_pin_clock, OUTPUT);
  pinMode(loop_pin_data, OUTPUT);
  Serial.begin(9600);
  Wire.begin();
  rtc.begin();
  //      rtc.adjust(DateTime(F(__DATE__),F(__TIME__)));

  if (!rtc.isrunning()) {
    Serial.println("Check your RTC!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

}

void loop() {
  //获得当前时间
  DateTime loopnow = rtc.now();

  //为sundata插入当前时间，并进行计算，获得日出时间、日落时间
  solarloop.time(loopnow.year(),loopnow.month(),loopnow.day(),loopnow.hour(),loopnow.minute(),loopnow.second());
  solarloop.calculations();
  loop_sunrise = solarloop.sunrise_time();
  Serial.print(loop_sunrise);
  Serial.print("--");
  loop_sunset = solarloop.sunset_time();
  Serial.print(loop_sunset);
  Serial.print("--");
  //  Serial.print(loopnow.minute());
  //    Serial.print("--");
  //当前时间，以十进制显示
loop_currenttime = 1.0*loopnow.hour() + 1.0*loopnow.minute() / 60 + 1.0*loopnow.second() / 3600;
//  loop_currenttime=8.0;
  loop_daytime =loop_sunset-loop_sunrise;
  loop_timelag = 1.0*loop_daytime/loop_leds;
  Serial.print(loop_daytime);
  Serial.print("--");
  Serial.print(loop_timelag);
  Serial.print("--");
  if(loop_currenttime>=loop_sunrise && loop_currenttime<loop_sunset){
    led_current = pow(2,round((loop_currenttime-loop_sunrise)/loop_timelag))+0.5;
  }
  else{
    led_current = 65535;
  }

  Serial.println(led_current);
  //数据导出
  digitalWrite(loop_pin_latch, LOW);
  shiftOut(loop_pin_data, loop_pin_clock, LSBFIRST, highByte(led_current));
  shiftOut(loop_pin_data, loop_pin_clock, LSBFIRST, lowByte(led_current));
  digitalWrite(loop_pin_latch, HIGH);
  delay(1000);
}

