#include <stdint.h>
#include <genesis.h>
#include "crc32.h"


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

#define CMD_NOCMD           0x00
#define CMD_RESETSTATUS     0x01
#define CMD_TESTWORDRAM     0x02
#define CMD_TESTBACKUPRAM   0x03
#define CMD_GIVEBANK0       0x04
#define CMD_GIVEBANK1       0x05
#define CMD_GIVE2M          0x06
#define CMD_2MMODE          0x07
#define CMD_INITC           0x08
#define CMD_WAITINTERRUPT   0x09
#define CMD_PROGTOWORD      0x0A
#define CMD_WORDZEROFILLING 0x0B
#define CMD_WORDFFFILLING   0x0C

#define STATUS_IDLE 0x00
#define STATUS_FILLING 0x01
#define STATUS_CHECKING 0x02
#define STATUS_PASSED 0x03
#define STATUS_INITC 0x04
#define STATUS_C_OK 0x05
#define STATUS_WAITINTERRUPT 0x06
#define STATUS_GREENLED 0x07
#define STATUS_REDLED 0x08
#define STATUS_CHECKREQ 0x09

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

enum{
     ROW_BUS=0,
     ROW_BIOS=1,
     ROW_BIOSNAME=2,
     ROW_MAINPROG=3,
     ROW_MAINDATA=7,
     ROW_SUBWORD=9,
     ROW_SUBBACKUP=10,
     ROW_MODE1M=11,
     ROW_CODE=13,
     ROW_INTERRUPT2=14,
     ROW_INTERRUPT3=15,
     ROW_ACTION=21,
     ROW_CMD=22,
     ROW_VARS=23
};


enum{
     FILLSTART_MAINPROG=0,
     FILLSTART_MAINDATA=5,
     FILLSTART_MODE1M=7
};

enum{
     COLUMN_FIRST=14,
     COLUMN_SECOND=18,
     COLUMN_THIRD=22
};

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


struct KnownCrc{
  uint32_t crc;
  const char* name;
};

static const struct KnownCrc knownCrcs[] =
  {
   { 0x0507b590,"G:MCDMod2V2.00(E)(Mar1993)[!]" },
   { 0x0f15d7e6,"N:megacd-2-e" },
   { 0x14dcbba9,"N:MegaCD(American)" },
   { 0x170ca9da,"G:CDXProV1.8I(Unl)" },
   { 0x2a4b82b5,"G:MCDMod2V2.00W(E)(Jun1993)[b2]" },
   { 0x2b19972f,"SR:WONDERMEGA-G303.BIN" },
   { 0x2e49d72c,"G:SCDMod2V2.11(U)[!]" },
   { 0x2ea250c0,"SR:MCDv1.00o(1991)(JP)(en-ja)" },
   { 0x340b4be4,"G:SCDMod2V2.00(U)[a1]" },
   { 0x39d897a9,"N:SCD_Art_Test" },
   { 0x39ebc967,"N:scd_110s" },
   { 0x433e44b5,"N:megacd-2-u" },
   { 0x4374808d,"N:MCDMod2V2.00c(J)(Dec1992)[b1]" },
   { 0x4a940d4a,"G:SCDXV2.21(U)[b1]" },
   { 0x4be18ff6,"SR:jp_mcd2_920501" },
   { 0x4d5cb8da,"G:MCDMod2V2.00W(E)(Jun1993)[!]" },
   { 0x4f639d2e,"N:megacd-1-e" },
   { 0x529ac15a,"G:MCDMod1V1.00(E)[!]" },
   { 0x550f30bb,"G:MCDMod1V1.00(J)[!]" },
   { 0x58c35bd3,"N:megacd-1-u" },
   { 0x58c94b97,"G:SCDMod1V1.10(U)[b1]" },
   { 0x62108fff,"G:MCDMod2V2.00(E)(Mar1993)[b2]" },
   { 0x6567ef38,"G:VisualSceneSCDImage(J)[b1]" },
   { 0x72f0d6db,"N:cdx!(robcd)(2013.08.3012.30)" },
   { 0x79f85384,"G:MCDMod1V1.005(J)" },
   { 0x7f7677cc,"G:SCDMod2V2.00WEmuHoax(Hack)" },
   { 0x8052c7a0,"SR:MPR-15768-T" },
   { 0x882fb4e5,"N:sonicdreams" },
   { 0x8af65f58,"G:SCDMod2V2.00(U)" },
   { 0x9b1ffc6f,"G:MCDMod2V2.00(E)(Mar1993)[b1]" },
   { 0x9bce40b2,"SR:MCDv1.00g(1991)(JP)(en-ja)" },
   { 0x9d2da8f2,"G:MCDMod1V1.01(J)(Nov1991)" },
   { 0x9f6f6276,"G:SCDMod2V2.00W(U)" },
   { 0xaacb851e,"SR:eu_mmg_930916" },
   { 0xb0519ed3,"G:SCDMod2V2.11(U)[b1]" },
   { 0xb501ded0,"SR:WONDERMEGA-G303_BAD_DUMP.BIN" },
   { 0xba7bf31d,"G:MCDMod2V2.00C(J)(Dec1992)[ab1]" },
   { 0xc6d10268,"G:SCDMod1V1.10(U)" },
   { 0xce60984e,"G:SCD68K(Hack)" },
   { 0xd21fe71d,"SR:jp_wmg_920206.BIN" },
   { 0xd344f125,"G:MCDMod2V2.00W(E)(Jun1993)[b1]" },
   { 0xd48c44b5,"G:SCDXV2.21(U)[!]" },
   { 0xd640553e,"G:CDXProV1.8I(Unl)[o1]" },
   { 0xd7423eb5,"N:cdx!(waltjag)(2013.08.2713.37)" },
   { 0xdd6cc972,"G:MCDMod2V2.00C(J)(Dec1992)[a1]" },
   { 0xe7e3afe2,"G:SCDMod1V1.00(U)" },
   { 0xe7f3a492,"G:CDXProV1.70(Unl)[b1]" },
   { 0xf116793c,"G:MCDMod2V2.00C(J)(Dec1992)" },
   { 0xf18dde5b,"G:MCDMod1V1.00l(J)" },
  };


struct State{
  uint8_t status;
  uint16_t memMode;
  uint16_t resetHalt;
};


struct State getState(){
  static uint8_t cnt=0;
  struct State s;
  s.status=*MAIN_COMM_STATUS;
  s.memMode=*MAIN_MEMMODE;
  s.resetHalt=*MAIN_RESETHALT;
  
  char buffer[33];
  sprintf(buffer,"%cRH %04x MM %04x ST %02x %s",
          rotatingBar[(cnt>>4)%4],
          s.resetHalt,
          s.memMode,
          s.status,getMsg(s.status));
  int l=strlen(buffer);
  memset(buffer+l,' ',32-l);
  buffer[32]=0;
  VDP_drawText(buffer, 0, ROW_VARS); 
  VDP_waitVSync();
  cnt++;
  return s;
}


void sendCmd(uint8_t cmd){
  char buffer[32];
  sprintf(buffer,"send cmd %02x",cmd);
  VDP_drawText(buffer, 0,ROW_CMD);
  *MAIN_COMM_CMD=cmd;
  while(getState().status!=STATUS_CMDREAD);
  sprintf(buffer,"exec cmd %02x",cmd);
  VDP_drawText(buffer, 0,ROW_CMD);
  *MAIN_COMM_CMD=CMD_NOCMD;
  VDP_waitVSync();
}




void printActionLine(const char* str){
  char buffer[33];
  memset(buffer,' ',32);
  buffer[32]=0;
  memcpy(buffer,str,strlen(str));
  VDP_drawText(buffer, 0, ROW_ACTION);
  VDP_waitVSync();
}



void askBank(uint8_t bank){
  char buffer[33];
  sprintf(buffer,"1M - ask bank %02x",bank);
  printActionLine(buffer);
  sendCmd(bank==0?CMD_GIVEBANK0:CMD_GIVEBANK1);
  uint16_t memMode; 
  do{
    memMode=getState().memMode;
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


void printFirstResult(uint8_t row,const char* header,bool success){
  VDP_drawText(header,0,row);
  VDP_drawText(success?"ok":"bad",COLUMN_FIRST,row);
  VDP_waitVSync();
}

void printResult(uint8_t row,uint8_t col,bool success){
  VDP_drawText(success?"ok":"bad",col,row);
  VDP_waitVSync();
}


/*void printMyself(){
  for (uint8_t i=0;i<9;i++){
    for (uint8_t j=0;j<0x10;j++){
      sprintf(buffer,"%02x",*(uint8_t*)(16*i+j));
      VDP_drawText(buffer,j*2, i);
    }
  }
}
*/


void testMainOnly(){
  char buffer[33];  
  VDP_drawText("req bus", 0, ROW_BUS);
  VDP_waitVSync();
  *MAIN_RESETHALT=SBRQ;
  struct State state;
  do{
    state=getState();
  }while ((!(state.resetHalt&SBRQ))||(state.memMode&DMNA));
  VDP_drawText("got bus", 14, ROW_BUS);


  initCrcTable();
  VDP_drawText("BIOS CRC32", 0, ROW_BIOS);
  VDP_waitVSync();
  uint32_t crc=0;
  crc32((void*)OFFSET,0x020000,&crc);
  void* crca=&crc;
  sprintf(buffer,"%04x%04x",*(uint16_t*)crca,*(((uint16_t*)crca)+1));
  VDP_drawText(buffer, COLUMN_FIRST, ROW_BIOS);
  VDP_waitVSync();

  bool found=false;
  for (uint16_t i=0;i<sizeof(knownCrcs)/sizeof(struct KnownCrc);i++){
    if (crc==knownCrcs[i].crc){
      found=true;
      VDP_drawText(knownCrcs[i].name,0, ROW_BIOSNAME);
      VDP_waitVSync();
      break;
    }
  }
  if (!found){
    VDP_drawText("Unknown BIOS",0, ROW_BIOSNAME);
  }


    
  for (uint8_t bank=0;bank<4;bank++){
    *MAIN_MEMMODE=(bank<<6)|RET;
    sprintf(buffer,"PROG BANK %d",bank);
    VDP_drawText(buffer, 0, bank+ROW_MAINPROG);
    VDP_waitVSync();
    write128K((uint8_t*)(OFFSET+0x20000),bank+FILLSTART_MAINPROG);
  }
    
    
  uint8_t r=0;
  for (uint8_t* x=(uint8_t*)(OFFSET+0x200000);x<(uint8_t*)(OFFSET+0x240000);x+=0x20000){
    sprintf(buffer,"DATA ADDR %x",(uint16_t)((uint32_t)x>>16) );
    VDP_drawText(buffer, 0, r+ROW_MAINDATA);
    VDP_waitVSync();
    write128K(x,r+FILLSTART_MAINDATA);
    r++;
  }
    
    
  for (uint8_t bank=0;bank<4;bank++){
    *MAIN_MEMMODE=(bank<<6)|RET;
    bool success=read128k((uint8_t*)(OFFSET+0x20000),bank+FILLSTART_MAINPROG);
    printResult(bank+ROW_MAINPROG,COLUMN_FIRST,success);
  }
  r=0;
    
  for (uint8_t* x=(uint8_t*)(OFFSET+0x200000);x<(uint8_t*)(OFFSET+0x240000);x+=0x20000){
    bool success=read128k(x,r+FILLSTART_MAINDATA);
    printResult(r+ROW_MAINDATA,COLUMN_FIRST,success);
    r++;
  }
            
}


static void stopSubCpu(){
  printActionLine("stop cpu");
  *MAIN_RESETHALT=(0<<6)|SBRQ;
  while( (getState().resetHalt&SBRQ)!=SBRQ);
}

static void startSubCpu(){
  printActionLine("start sub cpu");
  *MAIN_RESETHALT=(0<<6)|SRES;
  while( (getState().resetHalt&SBRQ)!=0);
}

void set2MMode(){
  printActionLine("set 2M mode");
  sendCmd(CMD_2MMODE);
  while(getState().memMode&MODE);
}

static void startSubCpuWithMem(){
  *MAIN_MEMMODE= DMNA;
  startSubCpu();
  set2MMode();
  printActionLine("wait sub mem acquisition");
  while(getState().memMode&RET){
    *MAIN_MEMMODE= DMNA;
  }
}

static void memToSub(){
  *MAIN_MEMMODE= DMNA;
  printActionLine("wait mem acquisition");
  while(getState().memMode&RET);
}

static bool checkCode(const uint8_t * prog){
  const uint8_t* src=(uint8_t*)&subCodeStart;
  uint16_t size=&subCodeEnd-&subCodeStart;
  bool success=true;
  for (uint16_t i=0;i<size;i++){
    if (prog[i]!=src[i]){
      success=false;
      break;
    }
  }
  return success;
}




static void testWithSub(){
  char buffer[33];
  uint8_t status=0;

  stopSubCpu();
  printActionLine("sub upload");
  
  *MAIN_MEMMODE=(0<<6)|RET;
  memcpy((void*)(OFFSET+0x20000),&subCodeStart,&subCodeEnd-&subCodeStart);
  
  
  startSubCpuWithMem();
  
  printActionLine("sub word ram test");
  sendCmd(CMD_TESTWORDRAM);

  do{
    status=getState().status;    
  }while(status!=STATUS_PASSED && status!=STATUS_ERROR);
  
  printFirstResult(ROW_SUBWORD,"SUB WORD",status==STATUS_PASSED);
  
  printActionLine("sub word FF filling");
  sendCmd(CMD_WORDFFFILLING);
  do{
    status=getState().status;    
  }while(status!=STATUS_PASSED && status!=STATUS_ERROR);
  printResult(ROW_SUBWORD,COLUMN_THIRD,status==STATUS_PASSED);

  printActionLine("sub word zero filling");
  sendCmd(CMD_WORDZEROFILLING);
  do{
    status=getState().status;    
  }while(status!=STATUS_PASSED && status!=STATUS_ERROR);
  printResult(ROW_SUBWORD,COLUMN_SECOND,status==STATUS_PASSED);

  
    
  printActionLine("sub backup ram test");
  sendCmd(CMD_TESTBACKUPRAM);

  do{
    status=getState().status;      
  }while(status!=STATUS_PASSED && status!=STATUS_ERROR);
    
  printFirstResult(ROW_SUBBACKUP,"SUB BACKUP",status==STATUS_PASSED);

  
  for (uint8_t bank=0;bank<2;bank++){
    askBank(bank);
    sprintf(buffer,"1M - fill bank %02x",bank);
    printActionLine(buffer);
    write128K((uint8_t*)(OFFSET+0x200000),bank+FILLSTART_MODE1M);
  }
  
  for (uint8_t bank=0;bank<2;bank++){
    bool success;
    askBank(bank);
    sprintf(buffer,"1M - check bank %02x",bank);
    printActionLine(buffer);
    success=read128k((uint8_t*)(OFFSET+0x200000),bank+FILLSTART_MODE1M);
    sprintf(buffer,"1M - BANK %01x",bank);    
    printFirstResult(ROW_MODE1M+bank,buffer,success);
  }

 

  stopSubCpu();
  
 
  printActionLine("verify code");
  bool success=checkCode((uint8_t*)(OFFSET+0x20000));
  printFirstResult(ROW_CODE,"VERIFY CODE",success);

  startSubCpuWithMem();
  
  printActionLine("sub copy prog to word");
  sendCmd(CMD_PROGTOWORD);

  while(getState().status!=STATUS_CHECKREQ);
     
  printActionLine("get 2M");
  sendCmd(CMD_GIVE2M);
  while((getState().memMode&RET)==0);

  printActionLine("checking prog to word copy");
  success=checkCode((uint8_t*)(OFFSET+0x200000));
  printResult(ROW_CODE,COLUMN_SECOND,success);

  memToSub();

  printActionLine("sub init c");
  
  sendCmd(CMD_INITC);
  while(getState().status!=STATUS_C_OK);

  
  printActionLine("sub intr run prog");

  sendCmd(CMD_WAITINTERRUPT);
  printActionLine("sub intr send 2");
  (*MAIN_RESETHALT)|=0x8100;
  printActionLine("sub intr show");  
  while(true){
    status=getState().status;
    sprintf(buffer,"INTERRUPT 2   %s",(*MAIN_COMMUNICATION)&0x20?"yes":"no ");
    VDP_drawText(buffer, 0, ROW_INTERRUPT2);
    VDP_waitVSync();
    sprintf(buffer,"INTERRUPT 3   %s",(*MAIN_COMMUNICATION)&0x40?"yes":"no ");
    VDP_drawText(buffer, 0, ROW_INTERRUPT3);
  }



  
}


int main()
{

  testMainOnly();
  testWithSub();
}
