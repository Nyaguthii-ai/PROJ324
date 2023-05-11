#include "VL53L0X.h"

VL53L0X::VL53L0X(PinName sda, PinName scl)
    : _i2c(sda, scl), _deviceAddress(VL53L0X_I2C_ADDRESS), _millimeters(0), _continuous(false)
{
    setAddressDefault();
}

void VL53L0X::setAddress(uint8_t new_addr)
{
    writeRegister(0x8A, 0x55);
    writeRegister(0x8B, 0xAA);
    writeRegister(0x8C, 0x5A);
    writeRegister(0x7F, 0x00);
    writeRegister(0x81, 0x01);
    writeRegister(0x80, 0x01);
    writeRegister(0x94, new_addr & 0x7F);
    writeRegister(0x83, readRegister(0x83) | 0x04);
    wait_us(1000);
    _deviceAddress = new_addr;
}

void VL53L0X::setAddressDefault()
{
    setAddress(VL53L0X_I2C_ADDRESS);
}

void VL53L0X::writeRegister(uint8_t reg, uint8_t value)
{
    char data[] = {(char)reg, (char)value};
    _i2c.write(_deviceAddress, data, 2);
}

uint8_t VL53L0X::readRegister(uint8_t reg)
{
    char data = (char)reg;
    _i2c.write(_deviceAddress, &data, 1);
    _i2c.read(_deviceAddress, &data, 1);
    return data;
}

uint16_t VL53L0X::readRangeContinuousMillimeters()
{
    while ((_continuous && !(readRegister(0x13) & 0x07)) || (!_continuous && !(readRegister(0x13) & 0x01)));
    _millimeters = (readRegister(0x14) << 8) | readRegister(0x13);
    return _millimeters;
}


