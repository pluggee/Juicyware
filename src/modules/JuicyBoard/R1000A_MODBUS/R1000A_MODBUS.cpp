/*
 * R1000A_I2C.cpp
 *
 *  Created on: Sep 17, 2016
 *      Author: sherifeid
 */

#include "mbed.h"
#include "R1000A_MODBUS.h"
#include "libs/Kernel.h"
#include "libs/Module.h"
#include "LPC1768/PinNames.h"

R1000A_MODBUS::R1000A_MODBUS(int slotnum, int baudrate, int databits, int stopbits, string _parity){
    // Default Constructor
 
    //this->serial = new mbed::Serial(p25, p26);
    //this->serial = new mbed::Serial(P2_1, P2_0);

    this->enabled = true;

    PinName tx_pin, rx_pin, dir_pin;
    switch (slotnum){
        // slots not working are commented out
        //case 1  : tx_pin = P1_30; rx_pin = P1_31; dir_pin = P0_23; break;
        case 2  : tx_pin = P2_0 ; rx_pin = P2_1 ; dir_pin = P2_2 ; break;
        //case 3  : tx_pin = P1_22; rx_pin = P1_23; dir_pin = P1_24; break;
        //case 4  : tx_pin = P1_26; rx_pin = P1_27; dir_pin = P1_28; break;
        //case 5  : tx_pin = P0_10; rx_pin = P0_11; dir_pin = P0_1 ; break;
        case 6  : tx_pin = P3_25; rx_pin = P2_11; dir_pin = P2_12; break;
        case 7  : tx_pin = P0_19; rx_pin = P0_20; dir_pin = P0_21; break;
        //case 8  : tx_pin = P0_2 ; rx_pin = P0_3 ; dir_pin = P0_26; break;
        //case 9  : tx_pin = P0_15; rx_pin = P0_16; dir_pin = P0_17; break;
        case 10 : tx_pin = P2_4 ; rx_pin = P2_5 ; dir_pin = P2_6 ; break;
        //case 11 : tx_pin = P1_18; rx_pin = P1_19; dir_pin = P1_20; break;
        //case 12 : tx_pin = P4_28; rx_pin = P4_29; dir_pin = P0_4 ; break;
        //case 13 : tx_pin = P1_1 ; rx_pin = P1_0 ; dir_pin = P3_26; break;
        //case 14 : tx_pin = P1_14; rx_pin = P1_15; dir_pin = P1_16; break;
        //case 15 : tx_pin = P1_4 ; rx_pin = P1_8 ; dir_pin = P1_9 ; break;
        default : tx_pin = NC   ; rx_pin = NC   ; dir_pin = NC   ; break;
    }
    
    this->serial = new BufferedSoftSerial(tx_pin, rx_pin);
    this->serial->baud(baudrate);

    if (_parity == "O"){
        this->serial->format(databits,serial->Parity::Odd,stopbits);
        calculate_delay(baudrate, databits, 1, stopbits);
    }
    else if (_parity == "E"){
        this->serial->format(databits,serial->Parity::Even,stopbits);
        calculate_delay(baudrate, databits, 1, stopbits);
    }
    else{
        this->serial->format(databits,serial->Parity::None,stopbits);
        calculate_delay(baudrate, databits, 0, stopbits);
    }

    dir_output = new GPIO(dir_pin);
    dir_output->output();
    dir_output->clear();
}

R1000A_MODBUS::~R1000A_MODBUS(){
    // Destructor
}

// Called when the module has just been loaded
void R1000A_MODBUS::on_module_loaded() {
    // We want to be called every time a new char is received
    serial->attach(this, &R1000A_MODBUS::on_serial_char_received, BufferedSoftSerial::RxIrq);
}

// Called on Serial::RxIrq interrupt, meaning we have received a char
void R1000A_MODBUS::on_serial_char_received(){
    buffer.push_back(serial->getc());
}

void R1000A_MODBUS::send_test_string()
{
    char telegram[8];
    telegram[0] = 'T';       // Slave address
    telegram[1] = 'E';             // Function code
    telegram[2] = 'S'; // Coil address MSB
    telegram[3] = 'T'; // Coil address LSB
    telegram[4] = '.';   // number of coils to read MSB
    telegram[5] = '.';   // number of coils to read LSB  
    telegram[6] = '\r';              // CRC LSB
    telegram[7] = '\n';       // CRC MSB
    dir_output->set();
    this->serial->write(telegram, 8);
    delay((int) ceil(50 + 8 * delay_time));
    dir_output->clear();
}

void R1000A_MODBUS::calculate_delay(int baudrate, int bits, int parity, int stop) {

    float bittime = 1000.0 / (float)baudrate;
    // here we calculate how long a byte with all surrounding bits take
    // startbit + number of bits + parity bit + stop bit
    delay_time = bittime * (1 + bits + parity + 1);
}

void R1000A_MODBUS::delay(unsigned int value) {
    uint32_t start = us_ticker_read(); // mbed call
    while ((us_ticker_read() - start) < value*1000) {
        THEKERNEL->call_event(ON_IDLE, this);
    }

}

void R1000A_MODBUS::read_coil(int slave_addr, int coil_addr, int n_coils){
    char telegram[8];
    unsigned int crc;
    telegram[0] = slave_addr;       // Slave address
    telegram[1] = 0x01;             // Function code
    telegram[2] = (coil_addr >> 8); // Coil address MSB
    telegram[3] = coil_addr & 0xFF; // Coil address LSB
    telegram[4] = (n_coils >> 8);   // number of coils to read MSB
    telegram[5] = n_coils & 0xFF;   // number of coils to read LSB
    crc = crc16(telegram, 6);       
    telegram[6] = crc;              // CRC LSB
    telegram[7] = (crc >> 8);       // CRC MSB
    dir_output->set();
    serial->write(telegram,8);
    //delay((int) ceil(50 + 8 * delay_time));
    dir_output->clear();
    // TODO: read reply from buffer and return it
    // add delay for slave to respond
    // read and return data from buffer
}

int R1000A_MODBUS::read_holding_register(int slave_addr, int reg_addr, int rdelay){
    char telegram[8];
    unsigned int crc;
    telegram[0] = slave_addr;       // Slave address
    telegram[1] = 0x03;             // Function code
    telegram[2] = (reg_addr >> 8);  // Register address MSB
    telegram[3] = reg_addr & 0xFF;  // Register address LSB
    telegram[4] = 0;                // number of coils to read MSB
    telegram[5] = 0x1;              // number of coils to read LSB
    crc = crc16(telegram, 6);       
    telegram[6] = crc;              // CRC LSB
    telegram[7] = (crc >> 8);       // CRC MSB
    dir_output->set();
    serial->write(telegram,8);
    delay((int) ceil(8 * delay_time));
    dir_output->clear();
    delay((int) ceil(rdelay + 8 * delay_time));            // readlion needs an extra 100ms delay!!

    int buffin[12];                 // oversized buffer
    for (int i = 0; i < 8; i++){
            // read characters from buffer
            if (serial->readable() != 0)
                buffin[i] = serial->getc();
        }
    return (buffin[3] << 8) | buffin[4];
}

void R1000A_MODBUS::write_coil(int slave_addr, int coil_addr, bool data){
    char telegram[8];
    unsigned int crc;
    telegram[0] = slave_addr;       // Slave address
    telegram[1] = 0x05;             // Function code
    telegram[2] = (coil_addr >> 8); // Coil address MSB
    telegram[3] = coil_addr & 0xFF; // Coil address LSB
    telegram[4] = 0x00;             // Data MSB
    telegram[5] = (data == true) ? 0xFF : 0x00; // Data LSB
    crc = crc16(telegram, 6);       
    telegram[6] = crc;              // CRC LSB
    telegram[7] = (crc >> 8);       // CRC MSB
    dir_output->set();
    serial->write(telegram,8);
    delay((int) ceil(50 + 8 * delay_time));
    dir_output->clear();
}


void R1000A_MODBUS::write_holding_register(int slave_addr, int reg_addr, int data){
    char telegram[8];
    unsigned int crc;
    telegram[0] = slave_addr;       // Slave address
    telegram[1] = 0x06;             // Function code
    telegram[2] = (reg_addr >> 8);  // Register address MSB
    telegram[3] = reg_addr;         // Register address LSB
    telegram[4] = (data >> 8);      // Data MSB
    telegram[5] = data;             // Data LSB
    crc = crc16(telegram, 6);       
    telegram[6] = crc;              // CRC LSB
    telegram[7] = (crc >> 8);       // CRC MSB
    dir_output->set();
    serial->write(telegram,8);
    delay((int) ceil(50 + 8 * delay_time));
    dir_output->clear();
}

unsigned int R1000A_MODBUS::crc16(char *data, unsigned int len) {
    
    static const unsigned short crc_table[] = {
    0X0000, 0XC0C1, 0XC181, 0X0140, 0XC301, 0X03C0, 0X0280, 0XC241,
    0XC601, 0X06C0, 0X0780, 0XC741, 0X0500, 0XC5C1, 0XC481, 0X0440,
    0XCC01, 0X0CC0, 0X0D80, 0XCD41, 0X0F00, 0XCFC1, 0XCE81, 0X0E40,
    0X0A00, 0XCAC1, 0XCB81, 0X0B40, 0XC901, 0X09C0, 0X0880, 0XC841,
    0XD801, 0X18C0, 0X1980, 0XD941, 0X1B00, 0XDBC1, 0XDA81, 0X1A40,
    0X1E00, 0XDEC1, 0XDF81, 0X1F40, 0XDD01, 0X1DC0, 0X1C80, 0XDC41,
    0X1400, 0XD4C1, 0XD581, 0X1540, 0XD701, 0X17C0, 0X1680, 0XD641,
    0XD201, 0X12C0, 0X1380, 0XD341, 0X1100, 0XD1C1, 0XD081, 0X1040,
    0XF001, 0X30C0, 0X3180, 0XF141, 0X3300, 0XF3C1, 0XF281, 0X3240,
    0X3600, 0XF6C1, 0XF781, 0X3740, 0XF501, 0X35C0, 0X3480, 0XF441,
    0X3C00, 0XFCC1, 0XFD81, 0X3D40, 0XFF01, 0X3FC0, 0X3E80, 0XFE41,
    0XFA01, 0X3AC0, 0X3B80, 0XFB41, 0X3900, 0XF9C1, 0XF881, 0X3840,
    0X2800, 0XE8C1, 0XE981, 0X2940, 0XEB01, 0X2BC0, 0X2A80, 0XEA41,
    0XEE01, 0X2EC0, 0X2F80, 0XEF41, 0X2D00, 0XEDC1, 0XEC81, 0X2C40,
    0XE401, 0X24C0, 0X2580, 0XE541, 0X2700, 0XE7C1, 0XE681, 0X2640,
    0X2200, 0XE2C1, 0XE381, 0X2340, 0XE101, 0X21C0, 0X2080, 0XE041,
    0XA001, 0X60C0, 0X6180, 0XA141, 0X6300, 0XA3C1, 0XA281, 0X6240,
    0X6600, 0XA6C1, 0XA781, 0X6740, 0XA501, 0X65C0, 0X6480, 0XA441,
    0X6C00, 0XACC1, 0XAD81, 0X6D40, 0XAF01, 0X6FC0, 0X6E80, 0XAE41,
    0XAA01, 0X6AC0, 0X6B80, 0XAB41, 0X6900, 0XA9C1, 0XA881, 0X6840,
    0X7800, 0XB8C1, 0XB981, 0X7940, 0XBB01, 0X7BC0, 0X7A80, 0XBA41,
    0XBE01, 0X7EC0, 0X7F80, 0XBF41, 0X7D00, 0XBDC1, 0XBC81, 0X7C40,
    0XB401, 0X74C0, 0X7580, 0XB541, 0X7700, 0XB7C1, 0XB681, 0X7640,
    0X7200, 0XB2C1, 0XB381, 0X7340, 0XB101, 0X71C0, 0X7080, 0XB041,
    0X5000, 0X90C1, 0X9181, 0X5140, 0X9301, 0X53C0, 0X5280, 0X9241,
    0X9601, 0X56C0, 0X5780, 0X9741, 0X5500, 0X95C1, 0X9481, 0X5440,
    0X9C01, 0X5CC0, 0X5D80, 0X9D41, 0X5F00, 0X9FC1, 0X9E81, 0X5E40,
    0X5A00, 0X9AC1, 0X9B81, 0X5B40, 0X9901, 0X59C0, 0X5880, 0X9841,
    0X8801, 0X48C0, 0X4980, 0X8941, 0X4B00, 0X8BC1, 0X8A81, 0X4A40,
    0X4E00, 0X8EC1, 0X8F81, 0X4F40, 0X8D01, 0X4DC0, 0X4C80, 0X8C41,
    0X4400, 0X84C1, 0X8581, 0X4540, 0X8701, 0X47C0, 0X4680, 0X8641,
    0X8201, 0X42C0, 0X4380, 0X8341, 0X4100, 0X81C1, 0X8081, 0X4040
    };

    unsigned char tmp;
    unsigned short crc = 0xFFFF;

    while(len--) {
        tmp = *data++ ^ crc;
        crc = crc >> 8;
        crc ^= crc_table[tmp];
    }

    return crc;

}