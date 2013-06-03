#ifndef IO_H
#define IO_H

#include <stdint.h>

struct bits { 
  uint8_t b0:1; 
  uint8_t b1:1; 
  uint8_t b2:1; 
  uint8_t b3:1; 
  uint8_t b4:1; 
  uint8_t b5:1; 
  uint8_t b6:1; 
  uint8_t b7:1; 
} __attribute__((__packed__));
 
#define BIT(name,pin,reg) ((*(volatile struct bits*)&reg##name).b##pin) 

#define INPUT 0
#define OUTPUT 1

#define READ  PIN
#define WRITE PORT

#endif
