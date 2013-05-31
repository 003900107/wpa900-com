

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
    sprintf(b,"������Ҫ��ѯ��ͨѶ�˿ڣ�");
    return 25;
  }
  else if(ShellCmdCmp(a,SET,strlen(SET)))
  {
    shellstate = PASS;
    sprintf(b,"���������룺");
    return 12;
  }
  else
  {
    sprintf(b,"��Ч���\r\n\n");
    return 13;
  }
}


uint32_t CmdGetPara(char *a,char *b,uint8_t len)
{
  if(ShellCmdCmp(a,COM1,strlen(COM1)))
  {
    shellstate = INIT;
    sprintf(b,"����1������ѯ��ϣ�\r\n\n");
    return 22;
  }
  else if(ShellCmdCmp(a,COM2,strlen(COM2)))
  {
    shellstate = INIT;
    sprintf(b,"����2������ѯ��ϣ�\r\n\n");
    return 22;
  }
  else if(ShellCmdCmp(a,COM3,strlen(COM3)))
  {
    shellstate = INIT;
    sprintf(b,"����3������ѯ��ϣ�\r\n\n");
    return 22;
  }
  else
  {
    shellstate = INIT;
    sprintf(b,"��Ч���\r\n\n");
    return 13;
  }
}

uint32_t CmdPass(char *a,char *b,uint8_t len)
{
  if(ShellCmdCmp(a,PASSWORD,strlen(PASSWORD)))
  {
    shellstate = ENTER;
    sprintf(b,"������Ҫ���õ�ͨѶ�˿ڣ�");
    return 24;
  }
  else
  {
    shellstate = INIT;
    sprintf(b,"�������\r\n\n");
    return 13;
  }
}


uint32_t CmdEnterPara(char *a,char *b,uint8_t len)
{
  if(ShellCmdCmp(a,COM1,strlen(COM1)))
  {
    shellstate = SET_COM1;
    sprintf(b,"�����ô���1������");
    return 18;
  }
  else if(ShellCmdCmp(a,COM2,strlen(COM2)))
  {
    shellstate = SET_COM2;
    sprintf(b,"�����ô���2������");
    return 18;
  }
  else if(ShellCmdCmp(a,COM3,strlen(COM3)))
  {
    shellstate = SET_COM3;
    sprintf(b,"�����ô���3������");
    return 18;
  }
  else
  {
    shellstate = INIT;
    sprintf(b,"��Ч���\r\n\n");
    return 13;
  }
}
            
uint32_t CmdSetCom1(char *a,char *b,uint8_t len)
{
  shellstate = INIT;
  sprintf(b,"����1����������ϣ�\r\n\n");
  return 22;
}

uint32_t CmdSetCom2(char *a,char *b,uint8_t len)
{
  shellstate = INIT;
  sprintf(b,"����2����������ϣ�\r\n\n");
  return 22;
}

uint32_t CmdSetCom3(char *a,char *b,uint8_t len)
{
  shellstate = INIT;
  sprintf(b,"����3����������ϣ�\r\n\n");
  return 22;
}


void CmdShellInit(void)
{
    shellstate=INIT;
}