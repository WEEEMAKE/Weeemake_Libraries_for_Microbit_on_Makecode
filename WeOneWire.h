#ifndef WeOneWire_H
#define WeOneWire_H

#include "pxt.h"
#include <cstdint>
#include <math.h>

MicroBitPin get_port(uint8_t index);
uint8_t one_wire_reset(uint8_t pinNum);
uint8_t one_wire_respond(uint8_t pinNum);
void one_wire_write_byte(uint8_t pinNum,uint8_t value);
uint8_t one_wire_read_byte(uint8_t pinNum);

#endif