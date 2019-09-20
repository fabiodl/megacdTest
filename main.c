#include <stdint.h>

#include <genesis.h>

extern uint8_t asmStart,asmEnd;

volatile uint16_t* MAIN_RESETHALT    =(volatile uint16_t*)0x0A12000;
volatile uint16_t* MAIN_MEMMODE      =(volatile uint16_t*)0x0A12002;

volatile uint16_t* MAIN_COMMUNICATION=(volatile uint16_t*)0x0A1200E;



#define SRES 1
#define SBRQ 2

#define RET   1
#define DMNA  2
#define true 1
#define false 0


void write128K(uint8_t* x,uint8_t initv){
  uint8_t v=initv;
  for (uint32_t i=0;i<0x20000;i++){  
   x[i]=v;
   v++;
   if (v==131)
     v=0;
  }
}

bool read128k(uint8_t* x,uint8_t initv){
  uint8_t v=initv;
  for (uint32_t i=0;i<0x20000;i++){  
    if (x[i]!=v){
      return false;
    }
    v++;
    if (v==131)
      v=0;
  }
  return true;
}

/*
  When a cart is inserted in the MD, the CD hardware is mapped to
  0x400000 instead of 0x000000. So the BIOS ROM is at 0x400000, the
  Program RAM bank is at 0x420000, and the Word RAM is at 0x600000.
*/


//#define OFFSET 0
#define OFFSET 0x400000

int main()
{
  char buffer[32];
  
  for (uint8_t bank=0;bank<4;bank++){
    *MAIN_MEMMODE=(bank<<6)|RET;
    sprintf(buffer,"PROG BANK %d",bank);
    VDP_drawText(buffer, 0, bank);
    VDP_waitVSync();
    write128K((uint8_t*)(OFFSET+0x20000),bank);
    }
  
    
  uint8_t r=5;
  for (uint8_t* x=(uint8_t*)(OFFSET+0x200000);x<(uint8_t*)(OFFSET+0x240000);x+=0x20000){
    sprintf(buffer,"DATA ADDR %x",(uint16_t)((uint32_t)x>>16) );
    VDP_drawText(buffer, 0, r);
    VDP_waitVSync();
    write128K(x,r);    
    r++;
  }
    
  
  for (uint8_t bank=0;bank<4;bank++){
    *MAIN_MEMMODE=(bank<<6)|RET;
    bool success=read128k((uint8_t*)(OFFSET+0x20000),bank);    
    VDP_drawText(success?"ok":"bad", 14, bank);
    VDP_waitVSync();
  }
  r=5;
  
  for (uint8_t* x=(uint8_t*)(OFFSET+0x200000);x<(uint8_t*)(OFFSET+0x240000);x+=0x20000){
    bool success=read128k(x,r);    
    VDP_drawText(success?"ok":"bad", 14, r);
    VDP_waitVSync();
    r++;
  }
  
  VDP_drawText("request bus", 0, 7);
  *MAIN_RESETHALT=SBRQ;
  do{
    sprintf(buffer,"%04x",*MAIN_RESETHALT);
    VDP_drawText(buffer, 14, 7);
    VDP_waitVSync();
  }while (!((*MAIN_RESETHALT)&SBRQ));
  VDP_drawText("got bus", 19, 7);

  *MAIN_MEMMODE=(0<<6)|RET;
  memcpy((void*)(OFFSET+0x20000),&asmStart,&asmEnd-&asmStart);

  /*uint8_t* readBack=(uint8_t*)(OFFSET+0x20000);

  for (int i=0;i<64;i++){
    sprintf(buffer,"%02x ",*readBack);
    readBack++;
    VDP_drawText(buffer, 3*(i%8), i/8);
   
    }*/
  
  
  *MAIN_MEMMODE= DMNA;
  *MAIN_RESETHALT=SRES;

  while(true){
    sprintf(buffer,"RESETHALT %04x",*MAIN_RESETHALT);
    VDP_drawText(buffer, 0, 9);
    sprintf(buffer,"COMMUNICATION %04x",*MAIN_COMMUNICATION);
    VDP_drawText(buffer, 0, 10);    
    VDP_waitVSync();
  };
  
  
}
