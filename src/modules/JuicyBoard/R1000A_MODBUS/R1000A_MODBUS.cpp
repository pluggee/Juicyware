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

R1000A_MODBUS::R1000A_MODBUS(){
    // Default Constructor
 
    //this->serial = new mbed::Serial(p25, p26);
    //this->serial = new mbed::Serial(P2_1, P2_0);
    
    this->serial = new BufferedSoftSerial(P2_0, P2_1);
    this->serial->baud(MODBUS_BAUD);
    this->serial->format(MODBUS_DATABITS, serial->Parity::None, MODBUS_STOPBITS);
}

R1000A_MODBUS::~R1000A_MODBUS(){
    // Destructor
}

// Called when the module has just been loaded
void R1000A_MODBUS::on_module_loaded() {
    // We want to be called every time a new char is received
    
    //this->serial->attach(this, &R1000A_MODBUS::on_serial_char_received, mbed::Serial::RxIrq);
    //query_flag= false;
    //halt_flag= false;

    // We only call the command dispatcher in the main loop, nowhere else
    //this->register_for_event(ON_MAIN_LOOP);
    //this->register_for_event(ON_IDLE);

    // Add to the pack of streams kernel can call to, for example for broadcasting
    //THEKERNEL->streams->append_stream(this);
}

// Called on R1000A_MODBUS::RxIrq interrupt, meaning we have received a char
void R1000A_MODBUS::on_serial_char_received(){
    while(this->serial->readable()){
        char received = this->serial->getc();
        /*
        if(received == '?') {
            query_flag= true;
            continue;
        }
        if(received == 'X'-'A'+1) { // ^X
            halt_flag= true;
            continue;
        }
        // convert CR to NL (for host OSs that don't send NL)
        if( received == '\r' ){ received = '\n'; }
        */

        this->buffer.push_back(received);
    }
}

int R1000A_MODBUS::puts(const char* s)
{
    //return fwrite(s, strlen(s), 1, (FILE*)(*this->serial));
    size_t n= strlen(s);
    for (size_t i = 0; i < n; ++i) {
        _putc(s[i]);
    }
    return n;
}

void R1000A_MODBUS::send_test_string()
{
    char telegram[8];
    telegram[0] = 'T';       // Slave address
    telegram[1] = 'E';             // Function code
    telegram[2] = 'S'; // Coil address MSB
    telegram[3] = 'T'; // Coil address LSB
    telegram[4] = '1';   // number of coils to read MSB
    telegram[5] = '2';   // number of coils to read LSB  
    telegram[6] = '\r';              // CRC LSB
    telegram[7] = '\n';       // CRC MSB
    this->serial->write(telegram, 8);
    //for (int i=0; i<8; i++){
    //    _putc(telegram[i]);
    //}
}

int R1000A_MODBUS::_putc(int c)
{
    return this->serial->putc(c);
}

int R1000A_MODBUS::_getc()
{
    return this->serial->getc();
}

/*

int R1000A_I2C::I2C_ReadREG(int slotnum, char REGAddr, char * data, int length){
    // perform burst register read
    int i;                                      // for loop variable
    char I2CAddr = getSlotI2CAdd(slotnum);      // get slot I2C address
    // set the register to access
    this->i2c->start();
    if (this->i2c->write(I2CAddr) != 1){        // check for slave ack
        // slave I2C is not acknowledging, exit function
        this->i2c->stop();
        return -1;
    }
    this->i2c->write(REGAddr);                  // register address
    this->i2c->stop();

    // read part
    this->i2c->start();
    this->i2c->write(I2CAddr | 0x01);           // slave I2C address, with read command
    for (i=0; i<length; i++){                   // loop over every byte
        data[i] = this->i2c->read(1);
    }
    this->i2c->read(0);                         // extra dummy read for mbed I2C to stop properly
    this->i2c->stop();
    return 0;
}

int R1000A_I2C::I2C_Read(int slotnum, char * data, int length){
    // perform burst register read
    int i;                                      // for loop variable
    char I2CAddr = getSlotI2CAdd(slotnum);      // get slot I2C address
    // set the register to access
    this->i2c->start();
    if (this->i2c->write(I2CAddr | 0x01) != 1){        // check for slave ack // slave I2C address, with read command
        // slave I2C is not acknowledging, exit function
        this->i2c->stop();
        return -1;
    }
    for (i=0; i<length; i++){                   // loop over every byte
        data[i] = this->i2c->read(1);
    }
    this->i2c->read(0);                         // extra dummy read for mbed I2C to stop properly
    this->i2c->stop();
    return 0;
}

int R1000A_I2C::I2C_WriteREG(int slotnum, char REGAddr, char * data, int length){
    // perform burst register read
    int i;                                      // for loop variable
    char I2CAddr = getSlotI2CAdd(slotnum);     // get slot I2C address
    // set the register to access
    this->i2c->start();
    if (this->i2c->write(I2CAddr) != 1){        // check for slave ack
        // slave I2C is not acknowledging, exit function
        this->i2c->stop();
        return -1;
    }
    this->i2c->write(REGAddr);                  // register address
    for (i=0; i<length; i++){
        this->i2c->write(data[i]);              // write data one by one
    }
    this->i2c->stop();
    return 0;
}

int R1000A_I2C::I2C_CheckAddr(char I2CAddr){
    // check if I2C address acknowledges
    // set the register to access
    this->i2c->start();
    if (this->i2c->write((I2CAddr << 1) | 0x01) != 1){        // check for slave ack
        // slave I2C is not acknowledging, exit function
        this->i2c->stop();
        return -1;
    }
    else
    {
        this->i2c->stop();
        return 0;
    }
}

int R1000A_I2C::I2C_CheckAck(int slotnum){
    // checks mounted in the given slot acknowledges
    // set the register to access

    char I2CAddr = getSlotI2CAdd(slotnum);     // get slot I2C address
    this->i2c->start();
    if (this->i2c->write(I2CAddr) != 1){        // check for slave ack
        // slave I2C is not acknowledging, exit function
        this->i2c->stop();
        return -1;
    }
    else{
        this->i2c->stop();
        return 0;
    }
}

int R1000A_I2C::I2C_CheckBLMode(int slotnum){
    // checks mounted in the given slot acknowledges
    // set the register to access
    // returns:
    //      0 : if module is in bootloader mode
    //     -1 : I2C error, module didn't ack
    //     -2 : module is not in bootloader mode

    char i2cbuf[2];

    if (I2C_ReadREG(slotnum,TGT_CMD_CHECK_BLSTAT,i2cbuf,1)==0){
        // now read BLSTAT register
        if (i2cbuf[0] == TGT_RSP_BL_MODE){

            return 0;       // module in bootloader mode
        }
        else{
            return -2;      // module not in bootloader mode
        }
    }
    else{
        return -1;          // I2C ack error
    }
}

char R1000A_I2C::getSlotI2CAdd(int slotnum){
    // returns I2C address of the specific slot
    // This slot numbers are ordered as follows
    // Slots 1~15 are pluggable modules
    // Slot 100 is reserved for the power monitor chip
    // Slot 200 is reserved for the EEPROM
    if (slotnum == PWRMON_SLOT){
        return (PWRMON_BASE << 1);
    }
    else if ((slotnum >= EEPROM_SLOT_BASE) && (slotnum < (EEPROM_SLOT_BASE + EEPROM_NUM_SLOTS))){
        // this returns the address of EEPROM section, limit is 4 sections x 256 bytes
        return ((EEPROM_I2C_BASE + slotnum - EEPROM_SLOT_BASE) << 1);
    }
    else{
        return ((R1000_I2C_BASE + slotnum - 1) << 1);
    }
}

void R1000A_I2C::enablemodI2C(void){
    modI2Cenabled = true;
}

void R1000A_I2C::disablemodI2C(void){
    modI2Cenabled = false;
}

int R1000A_I2C::ismodI2Cenabled(void){

    if (modI2Cenabled){
        return 1;
    }
    else{
        return 0;
    }
}

float R1000A_I2C::I2C_ReadREGfloat (int slotnum, char REGAddr){
    // This function reads 4 bytes from the give slot/register
    // processes the data and returns a 'float' value

    // first create a union to translate between values
    union tempdata {
        float f;
        char c[4];
    };

    char i2cbuf[4];
    union tempdata chtemp;

    if (I2C_ReadREG(slotnum,REGAddr,i2cbuf,4) == 0){
        chtemp.c[3] = i2cbuf[0];
        chtemp.c[2] = i2cbuf[1];
        chtemp.c[1] = i2cbuf[2];
        chtemp.c[0] = i2cbuf[3];
        return chtemp.f;
    }
    else{
        // I2C error, still need to return a value
        return 0;
    }
}

*/