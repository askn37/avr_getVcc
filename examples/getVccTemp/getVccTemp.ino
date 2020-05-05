#include <Arduino.h>
#include "avr_getVcc.h"

void setup (void) {
    while (!Serial);
    Serial.begin(9600);
}

void loop (void) {
    float vcc = getVcc();
    float tmp = getCoreTemperature();

    Serial.print(vcc);
    Serial.write(',');
    Serial.print(tmp - 273.15);
    Serial.println();
    delay(1000);
}

// end of code
