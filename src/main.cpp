#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include "max6675.h"

#define TARGET_TEMP 130
#define TEMP_INTERVAL 500 #ms
#define PIN_TEMP_DO 4
#define PIN_TEMP_CS 3
#define PIN_TEMP_CLK 2
#define PIN_RELAIS 8
#define PIN_SWITCH 6
#define DEGREE (char)223

#define PERCENT_UP 70
#define PERCENT_TIME 10000



LiquidCrystal_I2C lcd(/*address=*/0x27, /*columns=*/16, /*rows=*/2);
MAX6675 thermocouple(PIN_TEMP_CLK, PIN_TEMP_CS, PIN_TEMP_DO);

bool relaisState = false;
int currentTemp = 303;
char text[2][17];
unsigned long lastDebounceTime = millis();
unsigned long lastRelais = millis();

void printlcd() {
    for ( byte i = 0; i<2; i++) {
        lcd.setCursor(0, i);
        lcd.print(text[i]);
    }
}

void setup() {
    Serial.begin(9600);
    pinMode(PIN_RELAIS, OUTPUT);
    pinMode(PIN_SWITCH, INPUT);

    lcd.init();
    lcd.backlight();
}

void loop() {
    //read thermocoupled sensor every second
    if ((millis() - lastDebounceTime) > 500) {
        lastDebounceTime = millis();
        currentTemp = int(thermocouple.readCelsius());
        Serial.println(thermocouple.readCelsius());
    }

    if (digitalRead(PIN_SWITCH) && currentTemp < TARGET_TEMP) {
        if (relaisState && (millis() - lastRelais) > PERCENT_TIME / 100 * PERCENT_UP) {
            lastRelais = millis();
            relaisState = false;
        }
        if (!relaisState && (millis() - lastRelais) > PERCENT_TIME - (PERCENT_TIME / 100 * PERCENT_UP)) {
            lastRelais = millis();
            relaisState = true;
        }
    }
    digitalWrite(PIN_RELAIS, relaisState);

    sprintf(text[0], "current: %03d%c%c %c", currentTemp, DEGREE, 'C', (relaisState == true) ? '^' : '-');
    sprintf(text[1], "target : %03d%c%c", int(TARGET_TEMP), DEGREE, 'C');
    printlcd();
}
