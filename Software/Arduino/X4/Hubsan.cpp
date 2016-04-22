#include <Arduino.h>
#include "A7105.h"
#include "Hubsan.h"

A7105 a7105;

//Initialize the A7105 RF module
void Hubsan::initialize(void)
{
  a7105.enableSPI();
  a7105.initialize();
}


