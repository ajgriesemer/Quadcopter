#ifndef A7105_H
#define A7105_H

class A7105
{
  public:
    bool initialize(void);
    void reset(void);
    void enableSPI(void);
    
  private:
    const int kCS_Pin = 10;
    enum States {
      Sleep     = 0x80,
      Idle      = 0x90,
      Standby   = 0xA0,
      PLL       = 0xB0,
      RX        = 0xC0,
      TX        = 0xD0,
      RST_WRPTR = 0xE0,
      RST_RDPTR = 0xF0,
    };
    enum Registers {
      Mode         = 0x00,
      ModeControl  = 0x01,
      Calc         = 0x02,
      Fifo1        = 0x03,
      Fifo2        = 0x04,
      FifoData     = 0x05,
      ID           = 0x06,
      RCOsc1       = 0x07,
      RCOsc2       = 0x08,
      RCOsc3       = 0x09,
      CKO          = 0x0A,
      GPIO1        = 0x0B,
      GPIO2        = 0x0C,
      Clock        = 0x0D,
      DataRate     = 0x0E,
      PLL1         = 0x0F,
      PLL2         = 0x10,
      PLL3         = 0x11,
      PLL4         = 0x12,
      PLL5         = 0x13,
      TX1          = 0x14,
      TX2          = 0x15,
      Delay1       = 0x16,
      Delay2       = 0x17,
      RXData       = 0x18,
      RXGain1      = 0x19,
      RXGain2      = 0x1A,
      RXGain3      = 0x1B,
      RXGain4      = 0x1C,
      RSSITHold    = 0x1D,
      Adc          = 0x1E,
      Code1        = 0x1F,
      Code2        = 0x20,
      Code3        = 0x21,
      IFCalib1     = 0x22,
      IFCalib2     = 0x23,
      VCOCurCalib  = 0x24,
      VCOSBCalib1  = 0x25,
      VCOSBCalib2  = 0x26,
      BattDetect   = 0x27,
      TXTest       = 0x28,
      RXDEMTest1   = 0x29,
      RXDEMTest2   = 0x2A,
      CPC          = 0x2B,
      CrystalTest  = 0x2C,
      PLLTest      = 0x2D,
      VCOTest1     = 0x2E,
      VCOTest2     = 0x2F,
      IFAT         = 0x30,
      RScale       = 0x31,
      FilterTest   = 0x32
    };
    
    enum Register_Masks {
        IFCalib1_FBCF = 1 << 4,
        A7105_MASK_VBCF = 1 << 3,
    };
    enum TXPower {
        TX100uW = 0,
        TX300uW = 1,
        TX1mW   = 2,
        TX3mW   = 3,
        TX10mW  = 4,
        TX30mW  = 5,
        TX100mW = 6,
        TX150mW = 7
    };
    void write(enum Registers address, uint8_t data);
    void write(long id);
    void write(uint8_t *dpbuffer, uint8_t len, uint8_t channel);
    void write(enum States strobe_state);
    uint8_t read(enum Registers address);
    uint8_t read(uint8_t *dpbuffer, uint8_t len);
    void setPower(enum TXPower power);
};

#endif
