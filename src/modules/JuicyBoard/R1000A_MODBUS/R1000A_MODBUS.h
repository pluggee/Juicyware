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

#include "Serial.h" // mbed.h lib
#include "BufferedSoftSerial.h"
#include "libs/RingBuffer.h"

// This is an I2C wrapper class for low level I2C commands
class R1000A_MODBUS {
    public:
        // Default Constructor
        R1000A_MODBUS();

        // Destructor
        ~R1000A_MODBUS();

        void on_module_loaded();                        // 
        void on_serial_char_received();

        // low level I2C operations
        //int I2C_ReadREG(int, char, char*, int);         // burst read (using slotnum)
        //int I2C_WriteREG(int, char, char*, int);        // burst write (using slotnum)
        //int I2C_Read(int, char*, int);                  // burst read (using slotnum) current register
        //float I2C_ReadREGfloat(int, char);              // reads a floating number (4 bytes) from the given slot/register

        //int I2C_CheckAddr(char);                        // checks if I2C address acknowledges
        //int I2C_CheckAck(int);                          // checks if slot acknowledges
        //int I2C_CheckBLMode(int);                       // checks if the slot is in bootloader mode

        // enable/disable/status for module I2C functions
        // This is used to enable and module I2C functions other than module bootloader I2C
        //void enablemodI2C(void);
        //void disablemodI2C(void);
        //int ismodI2Cenabled(void);

        int _putc(int c);
        int _getc(void);
        int puts(const char*);

        void send_test_string();        //FIXME: delete this after test

        RingBuffer<char,256> buffer;                    // Receive buffer
        //mbed::Serial* serial;
        BufferedSoftSerial* serial;

        struct {
          bool query_flag:1;
          bool halt_flag:1;
        };

    private:
        // Member variables
        //mbed::I2C* i2c;                                 // i2c comm class
        //char getSlotI2CAdd(int);                        // returns I2C address from slot number
        //bool modI2Cenabled;                             // a flag to show if normal module I2C operation is enabled
};

#endif /* SRC_MODULES_JUICYBOARD_R1000A_I2C_R1000A_MODBUS_H_ */
