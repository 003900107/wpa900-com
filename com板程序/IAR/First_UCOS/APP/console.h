
#ifndef __CONSOLE_H_
#define __CONSOLE_H_

#include "stdint.h"

/* shellstate */
#define INIT       0x00
#define GETPARA    0x01
#define SETPARA    0x02
#define PASS   0x03
#define ENTER      0x04

#define SET_COM1   0x10
#define SET_COM2   0x11
#define SET_COM3   0x12

uint32_t CmdMatch(char *a,char *b,uint8_t len);
void CmdShellInit(void);

#endif