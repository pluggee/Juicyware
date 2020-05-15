/*
 * R1000A_I2C.h
 *
 *  Created on: Sep 17, 2016
 *      Author: sherifeid
 */

#ifndef SRC_MODULES_JUICYBOARD_R1000A_I2C_R1000A_MODBUS_H_
#define SRC_MODULES_JUICYBOARD_R1000A_I2C_R1000A_MODBUS_H_

#define MODBUS_BAUD         9600       // I2C frequency
#define MODBUS_DATABITS     8
#define MODBUS_STOPBITS     1       

#include "BufferedSoftSerial.h"
#include "libs/gpio.h"
#include <vector>
#include <string>

// This is an I2C wrapper class for low level I2C commands
class R1000A_MODBUS {
    public:
        R1000A_MODBUS(int slotnum, int baudrate, int databits, int stopbits, string _parity);
        ~R1000A_MODBUS();                       

        void on_module_loaded();     
        void on_serial_char_received();       
        void send_test_string();        //FIXME: delete this after test

        void calculate_delay(int baudrate, int bits, int parity, int stop);
        void delay(unsigned int);

        void read_coil(int slave_addr, int coil_addr, int n_coils);
        int read_holding_register(int slave_addr, int reg_addr, int rdelay);
        void write_coil(int slave_addr, int coil_addr, bool data);
        void write_holding_register(int slave_addr, int reg_addr, int data);
        unsigned int crc16(char *data, unsigned int len); 

    private:
        BufferedSoftSerial* serial;
        std::vector<int> buffer;
        GPIO *dir_output;
        float delay_time;
        bool enabled;
};

#endif /* SRC_MODULES_JUICYBOARD_R1000A_I2C_R1000A_MODBUS_H_ */
