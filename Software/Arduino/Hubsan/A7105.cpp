#include <Arduino.h>
#include <SPI.h>
#include "A7105.h"

void A7105::initialize(void)
{
  pinMode(kCS_Pin, OUTPUT);
  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  reset();
  
  write((long)0x55201041); //Set ID Code Register (x06) to "55 20 10 41".
  read(ID);
  write(ModeControl, 0x63); //Set Mode Control Register (x01) Auto RSSI measurement, Auto IF Offset, FIFO mode enabled, ADC Enabled
  write(Fifo1, 0x0f); //Set FIFO Register 1 (x03) - Set FIFO length to 16 bytes
  write(Clock, 0x05); //Set Clock Register (x0D) - Use Crystal Oscillator, CLK divider = /2
  write(DataRate, 0x04);
  write(TX2, 0x2b);
  write(RXData, 0x62);
  write(RXGain1, 0x80);
  write(RXGain4, 0x0A);
  write(Code1, 0x07);
  write(Code2, 0x17);
  write(DMTest1, 0x47);

  write(Standby); 
}
  
void A7105::write(enum Registers address, uint8_t data)
{
  digitalWrite(kCS_Pin, LOW);
  SPI.transfer((uint8_t)address); // spi_xfer(SPI2, address);
  SPI.transfer(data);    // spi_xfer(SPI2, data); 
  digitalWrite(kCS_Pin, HIGH);
}

void A7105::write(long id)
{
    digitalWrite(kCS_Pin, LOW);
    SPI.transfer((uint8_t)ID);
    SPI.transfer((id >> 24) & 0xFF);
    SPI.transfer((id >> 16) & 0xFF);
    SPI.transfer((id >> 8) & 0xFF);
    SPI.transfer((id >> 0) & 0xFF);
  digitalWrite(kCS_Pin, HIGH);
}

uint8_t A7105::read(enum Registers address)
{
    uint8_t data;
    int i;
    digitalWrite(kCS_Pin, LOW);
    // Bits A7-A0 make up the first uint8_t.
    // Bit A7 = 1 == Strobe.  A7 = 0 == access register.
    // Bit A6 = 1 == read.  A6 = 0 == write. 
    // bits 0-5 = address.
    SPI.transfer(0x40 | (uint8_t)address);
    data = SPI.transfer(0);
    digitalWrite(kCS_Pin, HIGH);
    return data;
}

//Set register 0x00 to the value of 0x00 to reset the device
//Wait for the device to reset
void A7105::reset(void)
{
    write(Mode, 0x00);
    delayMicroseconds(1000);
}
