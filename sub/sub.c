#include <stdint.h>

volatile uint16_t*  SUB_RESET =(volatile uint16_t*)0x0FF8000;
volatile uint8_t*   SUB_COMM =(volatile uint8_t*)0x0FF800F;

int main(){

     
  while(1){
    *SUB_RESET=0x101;
    for (uint32_t i=0;i<0x50000;i++){
      asm("");
    }
    *SUB_RESET=0x201;
    for (uint32_t i=0;i<0x50000;i++){      
      asm("");
    }

  }

}
