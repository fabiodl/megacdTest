#include <stdint.h>

volatile uint16_t*  SUB_RESET = (volatile uint16_t*)0x0FF8000;
volatile uint8_t*   SUB_COMM_STATUS =   (volatile uint8_t*)0x0FF800F;

volatile uint16_t*  TIMER3 =    (volatile uint16_t*)0x0FF8030;
volatile uint16_t*  INTERRUPTS =(volatile uint16_t*)0x0FF8032;

#define STATUS_WAITINTERRUPT 0x06
#define STATUS_GREENLED 0x07
#define STATUS_REDLED 0x08

uint8_t int2=0;
uint8_t int3=0;

void onInterrupt3(){
  int3=1;
}


void onInterrupt2(){
  int2=1;
}


static inline void setStatus(uint8_t base){
  *SUB_COMM_STATUS=base|
    (int3?0x40:0x00)|
    (int2?0x20:0x00)
    ;

}

void waitInterrupt(){

   *TIMER3=0xFF;
   *INTERRUPTS=0xFF;
  while(1){;
    *SUB_RESET=0x101;
    setStatus(STATUS_REDLED);
    for (uint32_t i=0;i<0x50000;i++){
      asm("");
    }
    *SUB_RESET=0x201;
    setStatus(STATUS_GREENLED);
    for (uint32_t i=0;i<0x50000;i++){      
      asm("");
    }     
    
  }

}
