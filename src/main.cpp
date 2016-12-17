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
bool _set = false;
bool _get = false;
bool _time = false;

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
            for(unsigned int i=0;i<nrEvents;i++)
            {
                Serial.println(events[i]);
            }
            Serial.println();
        }
        else if((_dataRec[0] == 'r') && (_dataRec[1] == 'e') && (_dataRec[2] == 's') && (_dataRec[3] == 'e') && (_dataRec[4] == 't'))
        {
            nrEvents = 0;
        }
        else if((_dataRec[0] == 'h') && (_dataRec[1] == 'e') && (_dataRec[2] == 'l') && (_dataRec[3] == 'p'))
        {
            Serial.println("Examples of valid commands:");
            Serial.println("get #prints all events");
            Serial.println("reset #resets event counter");

            Serial.println("s t 22:01 16.02.30 #set time to 22:01 2016-02-30");
            Serial.println("g t #get time currently set");
            Serial.println("");
        }
        else
        {
             if(_dataRec[0] == 's')
             {
                 _set = true;
             }
             else if(_dataRec[0] == 'g')
             {
                 _get = true;
             }
             else
             {
                 Serial.print("Command error\n");
             }

             if(_dataRec[2] == 't')
             {
                 _time = true;
             }
             else
             {
                 Serial.print("Command error\n");
             }

             if(_set && _time)
             {
                 int iHour = 0;
                 int iMinute = 0;
                 int iYear = 0;
                 int iMonth = 0;
                 int iDay = 0;

                 char tmpMin[3];
                 tmpMin[0] = _dataRec[7];
                 tmpMin[1] = _dataRec[8];
                 char tmpHour[3];
                 tmpHour[0] = _dataRec[4];
                 tmpHour[1] = _dataRec[5];
                 char tmpYear[3];
                 tmpYear[0] = _dataRec[10];
                 tmpYear[1] = _dataRec[11];
                 char tmpMonth[3];
                 tmpMonth[0] = _dataRec[13];
                 tmpMonth[1] = _dataRec[14];
                 char tmpDay[3];
                 tmpDay[0] = _dataRec[16];
                 tmpDay[1] = _dataRec[17];

                 iHour = atoi(tmpHour);
                 iMinute = atoi(tmpMin);
                 iYear = atoi(tmpYear);
                 iMonth = atoi(tmpMonth);
                 iDay = atoi(tmpDay);

                 if(((iHour >= 0) && (iHour <= 23)) && ((iMinute >= 0) && (iMinute <= 59)))

                 {
                     if(_time)
                     {
                         if(((iYear >= 0) && (iYear <= 99)) && ((iMonth >= 1) && (iMonth <= 12)) && ((iDay >= 1) && (iDay <= 31)))
                         {
                             clock.Hour = iHour;
                             clock.Minute = iMinute;
                             clock.Year = (iYear + 2000) - 1970;
                             clock.Month = iMonth;
                             clock.Day = iDay;

                             RTC.write(clock);
                         }
                     }
                 }
                 _set = false;
                 _time = false;
             }
             else if(_get && _time)
             {
                 Serial.print("Get Time: ");
                 Serial.print(zeroPad(clock.Hour,2));
                 Serial.print(":");
                 Serial.print(zeroPad(clock.Minute,2));
                 Serial.print(":");
                 Serial.print(zeroPad(clock.Second,2));

                 Serial.print(" ");
                 Serial.print(zeroPad(1970+clock.Year,4));
                 Serial.print("-");
                 Serial.print(zeroPad(clock.Month,2));
                 Serial.print("-");
                 Serial.print(zeroPad(clock.Day,2));
                 Serial.println();
                 _get = false;
                 _time = false;
             }

         }
         _stringComplete = false;
         ii = 0;
     }
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
