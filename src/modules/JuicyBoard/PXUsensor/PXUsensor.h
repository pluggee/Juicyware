/*
 * PXUsensor.h
 *
 *  Created on: May 13, 2020
 *      Author: Sherif Eid
 */

#ifndef SRC_MODULES_JUICYBOARD_PXUSENSOR_PXUSENSOR_H_
#define SRC_MODULES_JUICYBOARD_PXUSENSOR_PXUSENSOR_H_

#include "TempSensor.h"

class PXUsensor : public TempSensor{
    public:
        // Default Constructor
        PXUsensor();
        ~PXUsensor();

        void UpdateConfig(uint16_t module_checksum, uint16_t name_checksum);            // reads config file
        float get_temperature();                                                        // returns current_temp
        void set_temperature(float tempval);                                            // temperature value to set
        void pull_temperature();                                                        // forces temperature to be read from PXU

    private:
        float current_temp;
        int modbus_addr;
};


#endif /* SRC_MODULES_JUICYBOARD_PXUSENSOR_PXUSENSOR_H_ */
