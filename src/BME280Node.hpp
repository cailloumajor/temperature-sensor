/*
 * Homie node for BME280 sensor.
 */

#pragma once

#include <array>
#include <memory>

#include <Adafruit_BME280.h>
#include <Homie.hpp>

struct BME280Sensor {
    Adafruit_BME280& _bme;
    const char* _id = {};
    const char* _name = {};
    const char* _datatype = {};
    const char* _format = {};
    const char* _unit = {};

    BME280Sensor(Adafruit_BME280& bme);
    virtual float getValue() const = 0;
};

struct BME280TemperatureSensor : BME280Sensor {
    BME280TemperatureSensor(Adafruit_BME280& bme);
    float getValue() const override;
};

struct BME280PressureSensor : BME280Sensor {
    BME280PressureSensor(Adafruit_BME280& bme);
    float getValue() const override;
};

struct BME280HumiditySensor : BME280Sensor {
    BME280HumiditySensor(Adafruit_BME280& bme);
    float getValue() const override;
};

class BME280Node : public HomieNode {
public:
    BME280Node(
        const char* id,
        const char* name,
        uint8_t i2c_addr,
        unsigned measurementInterval);

protected:
    void setup() override;
    void loop() override;

private:
    uint8_t _addr;
    unsigned _interval;
    std::array<std::unique_ptr<BME280Sensor>, 3> _sensors;
    unsigned long _lastMeasurement = 0;
    Adafruit_BME280 _bme;
};
