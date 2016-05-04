#include <Arduino.h>
#include "A7105.h"
#include "Hubsan.h"

A7105 a7105;
const u8 allowed_channels[] = {0x14, 0x1e, 0x28, 0x32, 0x3c, 0x46, 0x50, 0x5a, 0x64, 0x6e, 0x78, 0x82};
const unsigned long session_id = 0x12345678;
const unsigned long transmitter_id = 0xdb042679;

enum BindStates
{
    BIND_1 = 1,
    BIND_2 = 2,
    BIND_3 = 3,
    BIND_4 = 4,
    BIND_5 = 5,
    BIND_6 = 6,
    BIND_7 = 7,
    BIND_8 = 8
};

//Initialize the A7105 RF module
void Hubsan::initialize(void)
{
  a7105.enableSPI();
  a7105.initialize();
  channel = allowed_channels[rand() % sizeof(allowed_channels)];
}

void Hubsan::bind(void)
{
  int i;
  u8 binding_packet[16];
  switch(state)
  {
    case BIND_1:
      buildBindPacket(binding_packet, state);
      a7105.write(Standby);
      a7105.write(binding_packet, 16, channel);
      delayMicroseconds(3000);
      for(i = 0; i < 20; i++)
      {
        if(!(a7105.read(Mode) & 0x01)) //TX bit in Mode register
          break;
      }
      if(i == 20)
        //Write failed
        return 1; 
      a7105.strobe(RX);
  }
}

void Hubsan::buildBindPacket(u8 * packet, u8 state)
{
    packet[0] = state;
    packet[1] = channel;
    packet[2] = (session_id >> 24) & 0xff;
    packet[3] = (session_id >> 16) & 0xff;
    packet[4] = (session_id >>  8) & 0xff;
    packet[5] = (session_id >>  0) & 0xff;
    packet[6] = 0x08;
    packet[7] = 0xe4;
    packet[8] = 0xea;
    packet[9] = 0x9e;
    packet[10] = 0x50;
    packet[11] = (transmitter_id >> 24) & 0xff;
    packet[12] = (transmitter_id >> 16) & 0xff;
    packet[13] = (transmitter_id >>  8) & 0xff;
    packet[14] = (transmitter_id >>  0) & 0xff;
    packet[15] = calculateChecksum
}

u8 const Hubsan::calculateChecksum(u8 * packet)
{
  int sum = 0;
  for(int i = 0; i < 15; i++)
    sum += packet[i];
  return (256 - (sum % 256)) & 0xff;
}


