/**
 * Blink
 *
 * Turns on an LED on for one second,
 * then off for one second, repeatedly.
 */
#include "Arduino.h"
#include <Time.h>
#include <Wire.h>
//#include <TimeAlarms.h>
#include <DS1307RTC.h>
#include "ToggleButton.h"
#include "Timer.h"


ToggleButton sensorIn(4);
tmElements_t clock;
Timer _clockTimer(1, 0);
String events[100];
unsigned int nrEvents = 0;
bool _stringComplete = false;
char _dataRec[30];

void setup()
{
  Serial.begin(9600);
  pinMode(4, INPUT);
  pinMode(5, OUTPUT);
//  clock.Hour = 22;
//  clock.Minute = 8;
//  clock.Year = (16 + 2000) - 1970;
//  clock.Month = 12;
//  clock.Day = 17;
//
//  RTC.write(clock);
  _clockTimer.Start();
}
String zeroPad(int val, int length)
{
    int valLength = 1;
    int valTemp = (val - val%10);
    String addZeros = "";

    while(true)
    {
        if(valLength >= length)
            break;
        if(valTemp <= 1)
            addZeros += "0";
        valTemp /= 10;
        valLength++;
    }
    char valCharArr[33];
    itoa(val,valCharArr,10);
    String valString = String(valCharArr);
    return addZeros+valString;
}
//#define SERIAL_DEBUG
void handleClock()
{

    if(_clockTimer.TimeIsUp())
    {
        if(RTC.read(clock))
        {
#ifdef SERIAL_DEBUG
            Serial.println("RTC read successfully");
            Serial.println(clock.Minute);
#endif
//            if(clock.Year > (2015-1970))
//                _clockOK = true;
//            else
//                _clockOK = false;
        }
        else
        {
//            _clockOK = false;
#ifdef SERIAL_DEBUG
            if(RTC.chipPresent())
            {
                Serial.println("The DS1307 is stopped.  Please run the SetTime");
                Serial.println("example to initialize the time and begin running.");
                Serial.println();
            }
            else
            {
                Serial.println("DS1307 read error!  Please check the circuitry.");
                Serial.println();
            }
#endif
        }

        _clockTimer.ResetTimer();
        _clockTimer.Start();
//        alarm.update(clock.Hour, clock.Minute);
    }
}
void handleSerial()
{
    static int ii = 0;

    while(Serial.available())
    {

        char inChar = (char)Serial.read();
        _dataRec[ii] = inChar;
        ii++;

        if(inChar == '\n')
        {
            _stringComplete = true;
            Serial.println("DONE");
            break;
        }
    }
    if(_stringComplete)
    {
        if((_dataRec[0] == 'g') && (_dataRec[1] == 'e') && (_dataRec[2] == 't'))
        {
            for(int i=0;i<nrEvents;i++)
            {
                Serial.println(events[i]);
            }
            Serial.println();
        }
        else if((_dataRec[0] == 'r') && (_dataRec[1] == 'e') && (_dataRec[2] == 's') && (_dataRec[2] == 'e') && (_dataRec[2] == 't'))
        {
            nrEvents = 0;
        }
    }
    _stringComplete = false;
}

void loop()
{
    handleSerial();
    handleClock();
  // turn the LED on (HIGH is the voltage level)
    digitalWrite(17,!digitalRead(4));
//  Serial.println(digitalRead(4));
    sensorIn.update();

    if(sensorIn.isButtonPressed())
    {
        String event = zeroPad(clock.Year+1970,4)
                + "-" + zeroPad(clock.Month,2)
                + " " + zeroPad(clock.Day,2)
                + "-" + zeroPad(clock.Hour,2)
                + "-" + zeroPad(clock.Minute,2)
                + "-" + zeroPad(clock.Second,2);
//        Serial.println(event);
        events[nrEvents++] = event;
        if(nrEvents > 98)
            nrEvents = 0;
//        for(int i=0;i<nrEvents;i++)
//        {
//            Serial.println(events[i]);
//        }
//        Serial.println();
    }
  // wait for a second
//  delay(100);
}
