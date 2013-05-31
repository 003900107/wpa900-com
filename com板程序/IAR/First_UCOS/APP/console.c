

#include "console.h"
#include "stdint.h"
#include "string.h"
#include <stdbool.h>

#define GET      "get"
#define SET      "set"
#define COM1     "com1"
#define COM2     "com2"
#define COM3     "com3"
#define PASSWORD "sac"

uint32_t shellstate;// NULL,GET,SET
uint32_t getstate;  //GET_COM1,GET_COM2,GET_COM3
uint32_t setstate;  //SET_COM1,SET_COM2,SET_COM3


bool ShellCmdCmp(char *a,const char *b,uint8_t len)
{
		if(0==strncmp(a,b,len))
			return (1);
		else
			return (0);
}

uint32_t CmdMatch(char *a,char *b,uint8_t len)
{
  uint8_t i,j;
  
  if(ShellCmdCmp(a,GET,strlen(GET)))
  {
    shellstate = GETPARA;
    sprintf(b,"请输入要查询的通讯端口：");
    return 25;
  }
  else if(ShellCmdCmp(a,SET,strlen(SET)))
  {
    shellstate = PASS;
    sprintf(b,"请输入密码：");
    return 12;
  }
  else
  {
    sprintf(b,"无效命令！\r\n\n");
    return 13;
  }
}


uint32_t CmdGetPara(char *a,char *b,uint8_t len)
{
  if(ShellCmdCmp(a,COM1,strlen(COM1)))
  {
    shellstate = INIT;
    sprintf(b,"串口1参数查询完毕！\r\n\n");
    return 22;
  }
  else if(ShellCmdCmp(a,COM2,strlen(COM2)))
  {
    shellstate = INIT;
    sprintf(b,"串口2参数查询完毕！\r\n\n");
    return 22;
  }
  else if(ShellCmdCmp(a,COM3,strlen(COM3)))
  {
    shellstate = INIT;
    sprintf(b,"串口3参数查询完毕！\r\n\n");
    return 22;
  }
  else
  {
    shellstate = INIT;
    sprintf(b,"无效命令！\r\n\n");
    return 13;
  }
}

uint32_t CmdPass(char *a,char *b,uint8_t len)
{
  if(ShellCmdCmp(a,PASSWORD,strlen(PASSWORD)))
  {
    shellstate = ENTER;
    sprintf(b,"请输入要设置的通讯端口：");
    return 24;
  }
  else
  {
    shellstate = INIT;
    sprintf(b,"密码错误！\r\n\n");
    return 13;
  }
}


uint32_t CmdEnterPara(char *a,char *b,uint8_t len)
{
  if(ShellCmdCmp(a,COM1,strlen(COM1)))
  {
    shellstate = SET_COM1;
    sprintf(b,"请设置串口1参数：");
    return 18;
  }
  else if(ShellCmdCmp(a,COM2,strlen(COM2)))
  {
    shellstate = SET_COM2;
    sprintf(b,"请设置串口2参数：");
    return 18;
  }
  else if(ShellCmdCmp(a,COM3,strlen(COM3)))
  {
    shellstate = SET_COM3;
    sprintf(b,"请设置串口3参数：");
    return 18;
  }
  else
  {
    shellstate = INIT;
    sprintf(b,"无效命令！\r\n\n");
    return 13;
  }
}
            
uint32_t CmdSetCom1(char *a,char *b,uint8_t len)
{
  shellstate = INIT;
  sprintf(b,"串口1参数设置完毕！\r\n\n");
  return 22;
}

uint32_t CmdSetCom2(char *a,char *b,uint8_t len)
{
  shellstate = INIT;
  sprintf(b,"串口2参数设置完毕！\r\n\n");
  return 22;
}

uint32_t CmdSetCom3(char *a,char *b,uint8_t len)
{
  shellstate = INIT;
  sprintf(b,"串口3参数设置完毕！\r\n\n");
  return 22;
}


void CmdShellInit(void)
{
    shellstate=INIT;
}