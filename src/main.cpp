#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Bounce2.h>
#include "max6675.h"
#include "CountDown.h"

//(address, columns, rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);
Bounce b = Bounce();

#define TARGET_TEMP 130
#define TEMP_DO 4
#define TEMP_CS 3
#define TEMP_CLK 2
#define RELAIS 8
#define SWITCH 6
#define START_COUNTDOWN 7
#define ANIMATION_TIME 500
#define DEGREE (char)223
#define PLAY (char)96
#define PAUSE (char)9208

byte play[] = {
  B00000,
  B01000,
  B01100,
  B01110,
  B01111,
  B01110,
  B01100,
  B01000
};
byte pause[] = {
  B00000,
  B01010,
  B01010,
  B01010,
  B01010,
  B01010,
  B01010,
  B01010
};


bool relaisState = false;
bool countdownStarted = false;
bool alerted = false;

MAX6675 thermocouple(TEMP_CLK, TEMP_CS, TEMP_DO);
int currentTemp = 303;
char current[17];
char target[17];
unsigned long lastDebounceTime = millis();


CountDown CD(CountDown::MINUTES);

void println(int line, String text) {
    lcd.setCursor(0, line);
    lcd.print(text);
}


void timeOver() {
    for (byte i =0; i<=4;i++){
        lcd.clear();
        delay(ANIMATION_TIME/2);
        println(0, ">>>>> time <<<<<");
        println(1, ">>>>> over <<<<<");
        delay(ANIMATION_TIME/2);
    }
}


void startup() {
    println(0, " PCB-Bugeleisen ");
    println(1, ">     lukn     <");
    delay(ANIMATION_TIME);
    println(1, ">>    lukn    <<");
    delay(ANIMATION_TIME);
    println(1, ">>>   lukn   <<<");
    delay(ANIMATION_TIME);
    println(1, ">>>>  lukn  <<<<");
    delay(ANIMATION_TIME);
    println(1, ">>>>> lukn <<<<<");
    delay(ANIMATION_TIME);
    lcd.clear();
}

void setup() {
    Serial.begin(9600);
    pinMode(RELAIS, OUTPUT);
    pinMode(SWITCH, INPUT);
    b.attach(START_COUNTDOWN, INPUT);
    b.interval(25);

    lcd.init();
    lcd.backlight();

    //startup();
    CD.start(0,0,0,10);
    CD.stop();
    lcd.createChar(0, play);
        lcd.createChar(1, pause);
}

void restartCd(){
    CD.start(0,0,0,10);
    CD.stop();
    alerted = false;
}

void loop() {
    b.update();
    //read thermocoupled sensor every second
    if ((millis() - lastDebounceTime) > 500) {
        lastDebounceTime = millis();
        //currentTemp = random(100);
        //currentTemp = thermocouple.readCelsius();
        currentTemp = int(thermocouple.readCelsius());
        Serial.println(thermocouple.readCelsius());
    }

    if (b.fell()) {
        if (CD.isRunning()){
            restartCd();
        } else {
            if (CD.remaining() == 0) {
                restartCd();
            } else {
                CD.cont();
            }
        }
    }


    if (!alerted && CD.remaining() == 0) {
        timeOver();
        alerted = true;
    }
    relaisState = digitalRead(SWITCH) && currentTemp < TARGET_TEMP;
    digitalWrite(RELAIS, relaisState);

    sprintf(current, "current: %03d%c%c %c",currentTemp , DEGREE, 'C', (relaisState == true) ? '^' : '-');
    sprintf(target,  "time:    %03d%c  ",int(CD.remaining()), 's');
    lcd.setCursor(15,1);
    if (CD.isRunning()) {
        lcd.write(0);
    } else {
        lcd.write(1);
    }


    println(0, current);
    println(1, target);
    //startCountdown();

}
