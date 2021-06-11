#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "max6675.h"

//(address, columns, rows)
LiquidCrystal_I2C lcd(0x27, 16, 2);

#define TARGET_TEMP 130
#define TEMP_DO 4
#define TEMP_CS 3
#define TEMP_CLK 2
#define RELAIS 8
#define SWITCH 6
#define START_COUNTDOWN 7
#define ANIMATION_TIME 500
#define DEGREE (char)223



bool relaisState = false;

MAX6675 thermocouple(TEMP_CLK, TEMP_CS, TEMP_DO);
int currentTemp = 303;
char current[17];
char target[17];
unsigned long lastDebounceTime = millis();



void println(int line, String text) {
    lcd.setCursor(0, line);
    lcd.print(text);
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
    lcd.createChar(0, play);
    lcd.createChar(1, pause);
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


    relaisState = digitalRead(SWITCH) && currentTemp < TARGET_TEMP;
    digitalWrite(RELAIS, relaisState);

    sprintf(current, "current: %03d%c%c %c",currentTemp , DEGREE, 'C', (relaisState == true) ? '^' : '-');
    sprintf(target,  "target :    %03d%c  ",int(TARGET_TEMP), 's');
    lcd.setCursor(15,1);
    if (CD.isRunning()) {
        lcd.write(0);
    } else {
        lcd.write(1);
    }


    println(0, current);
}
