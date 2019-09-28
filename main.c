#include <stdint.h>
#include <genesis.h>



extern uint8_t subCodeStart,subCodeEnd;

volatile uint16_t* MAIN_RESETHALT    =(volatile uint16_t*)0x0A12000;
volatile uint16_t* MAIN_MEMMODE      =(volatile uint16_t*)0x0A12002;

volatile uint16_t* MAIN_COMMUNICATION=(volatile uint16_t*)0x0A1200E;
volatile uint8_t* MAIN_COMM_CMD=      (volatile uint8_t*)0x0A1200E;
volatile uint8_t* MAIN_COMM_STATUS=      (volatile uint8_t*)0x0A1200F;

#define SRES 1
#define SBRQ 2


#define RET   1
#define DMNA  2
#define MODE  4

#define true 1
#define false 0

#define CMD_NOCMD 0
#define CMD_RESETSTATUS 1
#define CMD_TESTWORDRAM 2
#define CMD_TESTBACKUPRAM 3
#define CMD_GIVEBANK0 4
#define CMD_GIVEBANK1 5
#define CMD_GIVE2M 6
#define CMD_2MMODE 7
#define CMD_INITC 8
#define CMD_WAITINTERRUPT 9

#define STATUS_IDLE 0x00
#define STATUS_FILLING 0x01
#define STATUS_CHECKING 0x02
#define STATUS_PASSED 0x03
#define STATUS_INITC 0x04
#define STATUS_C_OK 0x05
#define STATUS_WAITINTERRUPT 0x06
#define STATUS_GREENLED 0x07
#define STATUS_REDLED 0x08
#define STATUS_ERROR 0x0E
#define STATUS_CMDREAD 0x80
#define STATUS_CMDEXEC 0x81

/*
  When a cart is inserted in the MD, the CD hardware is mapped to
  0x400000 instead of 0x000000. So the BIOS ROM is at 0x400000, the
  Program RAM bank is at 0x420000, and the Word RAM is at 0x600000.
*/


#define OFFSET 0
//#define OFFSET 0x400000



static const char* msgs[]=
  {
   "idle    ",  //0
   "filling ",  //1
   "checking",  //2
   "ok      ",  //3
   "c init  ",  //4
   "c ok    ",  //5
   "wait int",  //6
   "grn led ",  //7
   "red led ",  //8
   "",          //9
   "",          //A
   "",          //B
   "",          //C
   "",          //D
   "failed  "   //E
  };

const char* getMsg(uint8_t status){
  if (status==STATUS_CMDREAD){
    return "cmd read";
  }
  if (status==STATUS_CMDEXEC){
    return "cmd exec";
  }
  
  status=status&0x1F;
  if (status<0x0F){
    return msgs[status];
  };
  return "unknown ";
}

static const char* rotatingBar="|/-\\";

uint8_t getStatus(){
  static uint8_t cnt=0;
  uint8_t status=*MAIN_COMM_STATUS;
  char buffer[33];
  sprintf(buffer,"status %02x %s %c",status,getMsg(status),rotatingBar[(cnt>>4)%4]);
  int l=strlen(buffer);
  memset(buffer+l,' ',32-l);
  buffer[32]=0;
  VDP_drawText(buffer, 0, 23); 
  VDP_waitVSync();
  cnt++;
  return status;
}


void sendCmd(uint8_t cmd){
  char buffer[32];
  sprintf(buffer,"send cmd %02x",cmd);
  VDP_drawText(buffer, 0,21);
  *MAIN_COMM_CMD=cmd;
  uint8_t status;
  do{
    status=getStatus();
  }while(status!=STATUS_CMDREAD);
  sprintf(buffer,"exec cmd %02x",cmd);
  VDP_drawText(buffer, 0,21);
  *MAIN_COMM_CMD=CMD_NOCMD;
  VDP_waitVSync();
}




void printActionLine(const char* str){
  char buffer[33];
  memset(buffer,' ',32);
  buffer[32]=0;
  memcpy(buffer,str,strlen(str));
  VDP_drawText(buffer, 0, 22);
  VDP_waitVSync();
}



void askBank(uint8_t bank){
  char buffer[33];
  sprintf(buffer,"1M - ask bank %02x",bank);
  printActionLine(buffer);
  sendCmd(bank==0?CMD_GIVEBANK0:CMD_GIVEBANK1);
  uint16_t memMode;
  do{
    memMode=*MAIN_MEMMODE;
  }while( ((memMode&MODE)!=MODE)||((memMode&RET)!=bank));  
}

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




void testMainOnly(){
    char buffer[33];
    uint16_t resetHalt,memMode;
  
    VDP_drawText("req bus", 0, 0);
    VDP_waitVSync();
    *MAIN_RESETHALT=SBRQ;
    do{
      resetHalt=*MAIN_RESETHALT;
      memMode=*MAIN_MEMMODE;
      sprintf(buffer,"RH %04x MM %04x",resetHalt,memMode);
      VDP_drawText(buffer, 8, 0);
      VDP_waitVSync();
    }while ((!(resetHalt&SBRQ))||(memMode&DMNA));
    VDP_drawText("got bus", 30, 0);
    
    
    for (uint8_t bank=0;bank<4;bank++){
      *MAIN_MEMMODE=(bank<<6)|RET;
      sprintf(buffer,"PROG BANK %d",bank);
      VDP_drawText(buffer, 0, bank+1);
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
    VDP_drawText(success?"ok":"bad", 14, bank+1);
    VDP_waitVSync();
    }
    r=5;
    
    for (uint8_t* x=(uint8_t*)(OFFSET+0x200000);x<(uint8_t*)(OFFSET+0x240000);x+=0x20000){
      bool success=read128k(x,r);    
      VDP_drawText(success?"ok":"bad", 14, r);
    VDP_waitVSync();
    r++;
    }
            
}


static void stopSubCpu(){
 printActionLine("stop cpu");
 *MAIN_RESETHALT=(0<<6)|SBRQ;
 while( (*MAIN_RESETHALT&SBRQ)!=SBRQ);
}

void startCpu(){
  printActionLine("start cpu");
  *MAIN_RESETHALT=(0<<6)|SRES;
  while( (*MAIN_RESETHALT&SBRQ)!=0);
}


static void testWithSub(){
  char buffer[33];
  uint8_t status=0;
  uint16_t memMode;

  stopSubCpu();
  printActionLine("sub upload");
  
  *MAIN_MEMMODE=(0<<6)|RET;
  memcpy((void*)(OFFSET+0x20000),&subCodeStart,&subCodeEnd-&subCodeStart);
  
  
  *MAIN_MEMMODE= DMNA;
  startCpu();
    


  
  printActionLine("sub word ram test");
  sendCmd(CMD_TESTWORDRAM);

  do{
    status=getStatus();    
  }while(status!=STATUS_PASSED && status!=STATUS_ERROR);

  sprintf(buffer,"SUB WORD      %s",status==STATUS_PASSED?"ok":"bad");
  VDP_drawText(buffer, 0, 7);
  VDP_waitVSync();


  
  printActionLine("sub backup ram test");
  sendCmd(CMD_TESTBACKUPRAM);

  do{
    status=getStatus();      
  }while(status!=STATUS_PASSED && status!=STATUS_ERROR);
    
  sprintf(buffer,"SUB BACKUP    %s",status==STATUS_PASSED?"ok":"bad");
  VDP_drawText(buffer, 0, 8);
  VDP_waitVSync();

  
  for (uint8_t bank=0;bank<2;bank++){
    askBank(bank);
    sprintf(buffer,"1M - fill bank %02x",bank);
    printActionLine(buffer);
    write128K((uint8_t*)(OFFSET+0x200000),7+bank);
  }
  
 for (uint8_t bank=0;bank<2;bank++){
   bool success;
   askBank(bank);
   sprintf(buffer,"1M - check bank %02x",bank);
   printActionLine(buffer);
   success=read128k((uint8_t*)(OFFSET+0x200000),7+bank);
   sprintf(buffer,"1M - BANK %01x   %s",bank,success?"ok":"bad");
   VDP_drawText(buffer, 0, 9+bank);
 }

 

  stopSubCpu();
  
 
 printActionLine("verify code");
 const uint8_t* prog=(uint8_t*)(OFFSET+0x20000);
 const uint8_t* src=(uint8_t*)&subCodeStart;
 uint16_t size=&subCodeEnd-&subCodeStart;
 bool success=true;
 for (uint16_t i=0;i<size;i++){
   if (prog[i]!=src[i]){
     success=false;
     break;
   }
 }

 sprintf(buffer,"VERIFY CODE   %s",success?"ok":"bad");
 VDP_drawText(buffer, 0, 11);

 printActionLine("start sub cpu");
 *MAIN_RESETHALT=(0<<6)|SRES;
 while( (*MAIN_RESETHALT&SBRQ)!=0);
 
 printActionLine("set 2M mode");
 sendCmd(CMD_2MMODE);

 

 
  do{
    memMode=*MAIN_MEMMODE;
  }while( (memMode&MODE)!=0);
  
  printActionLine("sub init c");
  
  sendCmd(CMD_INITC);

  do{
    status=getStatus();
  }while(status!=STATUS_C_OK);

  printActionLine("sub intr run prog");

  sendCmd(CMD_WAITINTERRUPT);
  printActionLine("sub intr send 2");
  (*MAIN_RESETHALT)|=0x8100;
  printActionLine("sub intr show");  
  while(true){
    status=getStatus();
    sprintf(buffer,"INTERRUPT 2   %s",(*MAIN_COMMUNICATION)&0x20?"yes":"no ");
    VDP_drawText(buffer, 0, 12);    
    VDP_waitVSync();
    sprintf(buffer,"INTERRUPT 3   %s",(*MAIN_COMMUNICATION)&0x40?"yes":"no ");
    VDP_drawText(buffer, 0, 13);    
  }



  
}


int main()
{

  testMainOnly();
  testWithSub();
  
}
