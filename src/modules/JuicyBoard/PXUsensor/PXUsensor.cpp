/*
 * PXUsensor.cpp
 *
 *  Created on: May 13, 2020
 *      Author: Sherif Eid
 */

#include "PXUsensor.h"
#include "libs/Kernel.h"
#include "Config.h"
#include "checksumm.h"
#include "ConfigValue.h"

#define maddr_checksum CHECKSUM("maddr")
#define REG_PV  0           // address for process value readout
#define REG_SP  1           // address for setpoint

PXUsensor::PXUsensor(){
    // Default Constructor
    // do nothing
    current_temp = -1;       // initial value
}

PXUsensor::~PXUsensor(){
}

void PXUsensor::UpdateConfig(uint16_t module_checksum, uint16_t name_checksum){
    // load slot and channel numbers from config file
    modbus_addr = THEKERNEL->config->value(module_checksum, name_checksum, maddr_checksum)->by_default(1)->as_int();
}

float PXUsensor::get_temperature(){
    // returns set temperature
    return current_temp;
}

void PXUsensor::pull_temperature(){
    // returns set temperature
    int val = THEKERNEL->modbus->read_holding_register(modbus_addr, REG_PV);
    current_temp = (float)val/10.0;
}

void PXUsensor::set_temperature(float tempval){
    // sets temperature
    int tempint = ceil((int)(tempval * 10));
    THEKERNEL->modbus->write_holding_register(modbus_addr, REG_SP, tempint);
    // also reads current temperature
    int val = THEKERNEL->modbus->read_holding_register(modbus_addr, REG_PV);
    current_temp = (float)val/10.0;
}