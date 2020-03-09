#include <Arduino.h>
#include <Homie.h>

#include "BME280Node.hpp"

#define FIRMWARE_VERSION "1.0.4"

const uint8_t I2C_ADDRESS = 0x76;
const int INTERVAL = 60; // Measurement interval in seconds

BME280Node bmeNode("environment", "Environment sensor", I2C_ADDRESS, INTERVAL);

void setup()
{
    Serial.begin(115200);
    Serial << endl << endl;

    Homie_setFirmware("temperature-sensor", FIRMWARE_VERSION);

    Homie.setup();
}

void loop()
{
    Homie.loop();
}
