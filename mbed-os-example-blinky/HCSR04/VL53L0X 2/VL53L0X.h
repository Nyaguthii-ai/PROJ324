#ifndef VL53L0X_H
#define VL53L0X_H

#include "mbed.h"

#define VL53L0X_I2C_ADDRESS 0x52

class VL53L0X {
public:
    VL53L0X(PinName sda, PinName scl);

    uint16_t readRangeContinuousMillimeters();
    void startContinuous();

private:
    I2C _i2c;
    uint8_t _deviceAddress;
    uint16_t _millimeters;
    bool _continuous;
    
    void setAddress(uint8_t new_addr);
    void setAddressDefault();
    void writeRegister(uint8_t reg, uint8_t value);
    uint8_t readRegister(uint8_t reg);
};

#endif

