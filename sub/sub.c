#include <stdint.h>

volatile uint16_t*  SUB_RESET = (volatile uint16_t*)0x0FF8000;
volatile uint8_t*   SUB_COMM =   (volatile uint8_t*)0x0FF800F;

volatile uint16_t*  TIMER3 =    (volatile uint16_t*)0x0FF8030;
volatile uint16_t*  INTERRUPTS =(volatile uint16_t*)0x0FF8032;


uint8_t int2=0;
uint8_t int3=0;

void onInterrupt3(){
  int3=1;
}


void onInterrupt2(){
  int2=1;
}

int main(){
 *TIMER3=0xFF;
 *INTERRUPTS=0xFF;
 //int2=0;
  while(1){
    *SUB_RESET=0x101;
    for (uint32_t i=0;i<0x50000;i++){
      asm("");
    }
    *SUB_RESET=0x201;
    for (uint32_t i=0;i<0x50000;i++){      
      asm("");
    }
    (*SUB_COMM)|=
      (int3?0x40:0x00)|
      (int2?0x20:0x00)
      ;
    
  }

}
