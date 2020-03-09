/*
 * Homie node for BME280 sensor.
 */

#include "BME280Node.hpp"

using namespace HomieInternals;

BME280Sensor::BME280Sensor(Adafruit_BME280& bme)
    : _bme(bme)
    , _datatype("float")
{
}

BME280TemperatureSensor::BME280TemperatureSensor(Adafruit_BME280& bme)
    : BME280Sensor(bme)
{
    _id = "temperature";
    _name = "Temperature";
    _format = "0:65";
    _unit = "°C";
    _decimals = 1;
}

float BME280TemperatureSensor::getValue() const
{
    return _bme.readTemperature();
}

BME280PressureSensor::BME280PressureSensor(Adafruit_BME280& bme)
    : BME280Sensor(bme)
{
    _id = "pressure";
    _name = "Pressure";
    _format = "300:1100";
    _unit = "hPa";
    _decimals = 0;
}

float BME280PressureSensor::getValue() const
{
    return _bme.readPressure() / 100.0;
}

BME280HumiditySensor::BME280HumiditySensor(Adafruit_BME280& bme)
    : BME280Sensor(bme)
{
    _id = "humidity";
    _name = "Humidity";
    _format = "0:100";
    _unit = "%";
    _decimals = 1;
}

float BME280HumiditySensor::getValue() const
{
    return _bme.readHumidity();
}

BME280Node::BME280Node(
    const char* id,
    const char* name,
    uint8_t i2c_addr,
    unsigned measurementInterval)
    : HomieNode(id, name, "BME280")
    , _addr(i2c_addr)
    , _interval(measurementInterval)
{
    _sensors[0].reset(new BME280TemperatureSensor(_bme));
    _sensors[1].reset(new BME280PressureSensor(_bme));
    _sensors[2].reset(new BME280HumiditySensor(_bme));
}

void BME280Node::setup()
{
    Homie.getLogger() << endl << "🚀 Initializing BME280...";
    if (!_bme.begin(_addr)) {
        Helpers::abort(F("\n  ✖ Could not find a valid BME280 sensor"));
    }
    Homie.getLogger() << "OK!" << endl << endl;
    _bme.setSampling(
        Adafruit_BME280::MODE_FORCED,
        Adafruit_BME280::SAMPLING_X1,
        Adafruit_BME280::SAMPLING_X1,
        Adafruit_BME280::SAMPLING_X1,
        Adafruit_BME280::FILTER_OFF);
    for (const auto& s : _sensors) {
        advertise(s->_id)
            .setName(s->_name)
            .setDatatype(s->_datatype)
            .setFormat(s->_format)
            .setUnit(s->_unit);
    }
}

void BME280Node::loop()
{
    unsigned long currentMillis = millis();
    if ((unsigned long)(currentMillis - _lastMeasurement) >= (_interval * 1000UL)
        || (_lastMeasurement == 0 && Homie.isConnected())) {
        Homie.getLogger() << "🌡️ Sending BME280 measurements..." << endl;
        _bme.takeForcedMeasurement();
        for (const auto& s : _sensors) {
            String measure(s->getValue(), s->_decimals);
            Homie.getLogger() << "  • " << s->_name << ": " << measure << " "
                              << s->_unit << endl;
            setProperty(s->_id).send(measure);
        }
        _lastMeasurement = millis();
    }
}
