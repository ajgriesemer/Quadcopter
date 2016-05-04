#include <Arduino.h>
#include <SPI.h>
#include "A7105.h"

//Enable SPI port
void A7105::enableSPI(void)
{
  pinMode(kCS_Pin, OUTPUT);    //Configure chip select pin as output
  SPI.begin();                 //Open SPI port
  SPI.setDataMode(SPI_MODE0);  //Configure SPI port data mode
  SPI.setBitOrder(MSBFIRST);   //Configure SPI port bit order
}

bool A7105::initialize(void)
{
  int IF_calibration_loops = 0;
  uint8_t IF_calibration_1 = 0;
  int VCO_calibration_loops_1 = 0;
  uint8_t VCO_calibration_1 = 0;
  int VCO_calibration_loops_2 = 0;
  uint8_t VCO_calibration_2 = 0;
  bool success = 0;
  
  reset();  //Reset the A7105

  //Set A7105 configuration registers
  write((long)0x55201041); //Set ID Code Register (x06) to "55 20 10 41".
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
  write(RXDEMTest1, 0x47);

  
  /**********************Calibrate IF Filter Bank****************************/
  //Switch to Standby mode for calibration. The datasheet does say put it into PLL mode
  write(Standby);
  
  write(Calc, 0x01);       //Set FBC bit to calibrate IF Filter Bank
  delayMicroseconds(256);  //A7105 datasheet says the maximum calibration time should be 256 us
  while (read(Calc))       //Read FBC (bit 0). Bit is auto cleared when calibration is complete
  {
    if (IF_calibration_loops++ > 10)
      break;
  }
  IF_calibration_1 = read(IFCalib1); //Bit 4 is calibration failed bit
  if(IF_calibration_1 & IFCalib1_FBCF)
  {
    //IF calibration failed. Exit immediately
    return  1;
  }
  /****************************Calibrate VCO - PLL = 0**********************************/  
  //Set PLL1 Channel
  write(PLL1, 0x00);
  //Switch to PLL mode for calibration.
  write(PLL);
  
  write(Calc, 0x02);
  while (read(Calc)) //Bit is auto cleared when calibration is complete
  {
    if (VCO_calibration_loops_1++ > 10)
      break;
  }
  VCO_calibration_1 = read(VCOSBCalib1); //Bit 4 is calibration failed bit
  if(VCO_calibration_1 & VCOSBCalib1_VBCF)
  {
    //IF calibration failed. Exit immediately
    return  1;
  } 
  
  /****************************Calibrate VCO - PLL = 0x78**********************************/  
  //Set PLL1 Channel
  write(PLL1, 0x78);
  //Switch to PLL mode for calibration.
  write(PLL);
  
  write(Calc, 0x02);
  while (read(Calc)) //Bit is auto cleared when calibration is complete
  {
    if (VCO_calibration_loops_2++ > 10)
      break;
  }
  VCO_calibration_2 = read(VCOSBCalib1); //Bit 4 is calibration failed bit
  if(VCO_calibration_2 & VCOSBCalib1_VBCF)
  {
    //IF calibration failed. Exit immediately
    return  1;
  }   
  
  //Set VCO Single Band Manual Calibration Settings
  write(VCOSBCalib1, 0x0B);
  
  setPower(TX150mW);

  //Switch to Standby Mode
  write(Standby);
  return 0;
}

void A7105::write(uint8_t *  buffer, uint8_t len, uint8_t channel)
{
  digitalWrite(kCS_Pin, LOW);
  SPI.transfer(RST_WRPTR);
  SPI.transfer(FifoData);
  for( int i = 0; i < len; i++)
  {
    SPI.transfer(buffer[i]);
  }
  digitalWrite(kCS_Pin, HIGH);
  
  write(PLL1, channel); //set the channel
  
  write(TX); //strobe command to send the data
  
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

void A7105::write(enum States strobe_state)
{
  digitalWrite(kCS_Pin, LOW);
  SPI.transfer(strobe_state);
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

void A7105::setPower(enum TXPower power)
{
    /*
    Power amp is ~+16dBm so:
    TXPOWER_100uW  = -23dBm == PAC=0 TBG=0
    TXPOWER_300uW  = -20dBm == PAC=0 TBG=1
    TXPOWER_1mW    = -16dBm == PAC=0 TBG=2
    TXPOWER_3mW    = -11dBm == PAC=0 TBG=4
    TXPOWER_10mW   = -6dBm  == PAC=1 TBG=5
    TXPOWER_30mW   = 0dBm   == PAC=2 TBG=7
    TXPOWER_100mW  = 1dBm   == PAC=3 TBG=7
    TXPOWER_150mW  = 1dBm   == PAC=3 TBG=7
    */
    uint8_t pac, tbg;
    switch(power) {
        case 0: pac = 0; tbg = 0; break;
        case 1: pac = 0; tbg = 1; break;
        case 2: pac = 0; tbg = 2; break;
        case 3: pac = 0; tbg = 4; break;
        case 4: pac = 1; tbg = 5; break;
        case 5: pac = 2; tbg = 7; break;
        case 6: pac = 3; tbg = 7; break;
        case 7: pac = 3; tbg = 7; break;
        default: pac = 0; tbg = 0; break;
    };
    write(TXTest, (pac << 3) | tbg);
}
