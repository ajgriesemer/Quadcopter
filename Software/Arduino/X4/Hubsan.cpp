#include <Arduino.h>
#include "A7105.h"
#include "Hubsan.h"

A7105 a7105;
const u8 allowed_ch[] = {0x14, 0x1e, 0x28, 0x32, 0x3c, 0x46, 0x50, 0x5a, 0x64, 0x6e, 0x78, 0x82};
unsigned long sessionid;

enum BindStates
{
    BIND_1,
    BIND_2,
    BIND_3,
    BIND_4,
    BIND_5,
    BIND_6,
    BIND_7,
    BIND_8
};

//Initialize the A7105 RF module
void Hubsan::initialize(void)
{
  a7105.enableSPI();
  a7105.initialize();
  sessionid = rand();
  channel = allowed_ch[rand() % sizeof(allowed_ch)];
}

void Hubsan::bind(void)
{
  u8 binding_packet[16];
  switch(state)
  {
    case
  }
}

void Hubsan::buildBindPacket(u8 * packet, unsigned long transmitter_id, unsigned long session_id)
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


