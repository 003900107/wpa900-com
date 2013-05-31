
#include "includes.h"
#include "modbus_rtu_master.h"


static INT8U HI(INT16U data)
{
  return ((data>>8)&0xFF);
}

static INT8U LOW(INT16U data)
{
  return (data&0xFF);
}

#define READ_COIL     01
#define READ_DI       02
#define READ_HLD_REG  03
#define READ_AI       04
#define SET_COIL      05
#define SET_HLD_REG   06
#define READ_FIFO     24
#define PROTOCOL_EXCEPTION 0x81
#define PROTOCOL_ERR  1
#define FRM_ERR       2


const unsigned char auchCRCHi[] = {
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
0x40
} ;

const unsigned  char auchCRCLo[] = {
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
0x40
};


const unsigned char char_tab[128]=
{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                       
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                      
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                       
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0,                      
    0, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0, 0, 0, 0, 0, 0, 0, 0, 0,    
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,                      
    0, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0, 0, 0, 0, 0, 0, 0, 0, 0,    
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0                        
};
   
    
const unsigned char tab_char[16]=
{ 
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
};

const INT16U bit_tab[]=
{
    BIT0, BIT1, BIT2, BIT3, BIT4, BIT5, BIT6, BIT7, 
    BIT8, BIT9, BITA, BITB, BITC, BITD, BITE, BITF
};

unsigned short crc(unsigned char *puchMsg , unsigned short usDataLen)
{
    unsigned char uchCRCHi = 0xFF ; /* high byte of CRC initialized */
    unsigned char uchCRCLo = 0xFF ; /* low byte of CRC initialized */
    unsigned uIndex ;  /* will index into CRC lookup table */
    
    while (usDataLen--) /* pass through message buffer */
    {
    
   
     uIndex = uchCRCHi ^ *puchMsg++ ; /* calculate the CRC */
     uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex];
     uchCRCLo = auchCRCLo[uIndex] ;
    }
    return (uchCRCHi << 8 | uchCRCLo) ;
}

unsigned char a2toh(unsigned char *str)
{
    unsigned char tmp;
    tmp = 0;
    
    tmp = char_tab[*str];
    str++;
    tmp = tmp*16 + char_tab[* str];
    return tmp;
}

int get_bit( INT16U data, INT16U bit )
{
    if (bit > 16) return 0;
    if (data & bit_tab[bit]) return 1;
    else return 0;
}

void htoa(  char *str, unsigned char num)
{
    unsigned char tmp;
    tmp = num; 
    tmp = tmp & 0xf0;
    tmp >>= 4;
    *str = tab_char [ tmp];
    str++;
    *str = tab_char [ num & 0x0f];
}




void construct_rtu_frm     ( unsigned char *dst_buf,unsigned char *src_buf,unsigned char lenth)
{
    unsigned short  crc_tmp;
    crc_tmp = crc(src_buf, lenth);
     
    *(src_buf+lenth) = crc_tmp >> 8 ;
    *(src_buf+lenth+1) = crc_tmp & 0xff;
    lenth++;
    lenth++;
 
    while ( lenth--)
    {
       *dst_buf = *src_buf;
       dst_buf++;
       src_buf++; 
    }
}

/*
1 读取继电器状态
发送：
*/

INT16U rtu_getCoilStatus(INT8U addr,INT8U *buf,INT16U start_address,INT16U lenth) 
{
    unsigned char tmp[256], tmp_lenth;
    
    tmp[0] = addr;
    tmp[1] = READ_COIL;
    tmp[2] = HI(start_address);
    tmp[3] = LOW(start_address);
    tmp[4] = HI(lenth);
    tmp[5] = LOW(lenth);
    tmp_lenth = 6;
    construct_rtu_frm ( buf, tmp, tmp_lenth);
    return 8;
}

/*
2 读取开关量输入
发送：
*/


INT16U rtu_getInputStatus(INT8U addr,INT8U *buf,INT16U start_address,INT16U lenth) 
{
    unsigned char tmp[256], tmp_lenth;
    
    tmp[0] = addr;
    tmp[1] = READ_DI;
    tmp[2] = HI(start_address);
    tmp[3] = LOW(start_address);
    tmp[4] = HI(lenth);
    tmp[5] = LOW(lenth);
    tmp_lenth = 6;
    construct_rtu_frm ( buf,tmp,tmp_lenth);
    return 8;
}


/*
3 读取保持寄存器
发送：
*/

INT16U  rtu_getHoldingRegisters(INT8U addr,INT8U *buf,INT16U start_address,INT16U lenth) 
{
    unsigned char tmp[256], tmp_lenth;
    
    tmp[0] = addr;
    tmp[1] = READ_HLD_REG;
    tmp[2] = HI(start_address);
    tmp[3] = LOW(start_address);
    tmp[4] = HI(lenth);
    tmp[5] = LOW(lenth);
    tmp_lenth = 6;
    construct_rtu_frm ( buf,tmp,tmp_lenth);
    return 8;
}


// 4 发送读取模拟量输入

INT16U rtu_getAnalogInput(INT8U addr,INT8U *buf,INT16U start_address,INT16U lenth)
{
unsigned char tmp[256],tmp_lenth;
    tmp[0] = addr;
    tmp[1] = READ_AI;
    tmp[2] = HI(start_address);
    tmp[3] = LOW(start_address);
    tmp[4] = HI(lenth);
    tmp[5] = LOW(lenth);
    tmp_lenth = 6;
    construct_rtu_frm ( buf,tmp,tmp_lenth);
    return 8;
}


/*
5 设置继电器
发送：status =0 继电器释放 否则继电器吸合,address 为吸合的继电器编号，0为第一个继电器，依次类推
*/

INT16U  rtu_setCoil(INT8U addr,INT8U *buf,INT16U start_address,INT16U status)
{
    unsigned char tmp[256], tmp_lenth;
    
    tmp[0] = addr;
    tmp[1] = SET_COIL ;
    tmp[2] = HI(start_address);
    tmp[3] = LOW(start_address);

    if ( status )
    {
       tmp[4] = 0xff;
       tmp[5] = 0;
    }
    else
    {
       tmp[4]= 0;
       tmp[5]= 0;
    } 

    tmp_lenth = 6;
    construct_rtu_frm ( buf, tmp, tmp_lenth);
    return 8 ;
}


/*
6 设置保持寄存器
*/

INT16U rtu_setHoldingRegister(INT8U addr,INT8U *buf, INT16U start_address,INT16U value )
{
    unsigned char tmp[256], tmp_lenth;
    
    tmp[0] = addr;
    tmp[1] = SET_HLD_REG;
    tmp[2] = HI(start_address);
    tmp[3] = LOW(start_address);
    tmp[4] = HI(value);
    tmp[5] = LOW(value);    
    tmp_lenth = 6;
    construct_rtu_frm ( buf, tmp, tmp_lenth);
    return 8 ;
}

/*
7
接收分析：
dest_p 接收到数据指针
sourc_p 串口接收缓冲区指针
data_start_address 开始地址
*/

/* RTU  接收分析 */

INT16U rtu_data_anlys( int  *dest_p, unsigned char *source_p,     INT16U data_start_address, INT16U fr_lenth)
{
    unsigned short crc_result, crc_tmp;
    unsigned char tmp1, tmp2, shift;
   
    crc_tmp = *(source_p + fr_lenth-2); // crc  第一字节
    crc_tmp = crc_tmp * 256 + *( source_p+fr_lenth-1); // CRC 值
    crc_result = crc(source_p, fr_lenth-2); // 计算CRC 值
    
   
    if ( crc_tmp != crc_result ) // CRC 校验正确
    {
       //hld_reg[0x31]++;
       return -1;
    }
    switch ( *(source_p+1) ) // 功能码
    {
       case READ_COIL:                   //读取继电器状态 
       for ( tmp1=0; tmp1<*( source_p+2); tmp1++)
       {
            shift = 1;
            for ( tmp2=0; tmp2<8; tmp2++)
            { 
              //*( source_p+3)只是第一个字节，后面的字节呢？应该是*( source_p+tmp1+3)
                 *(dest_p+data_start_address+tmp1*8+tmp2) = shift & *( source_p+3);
                 *( source_p+3) >>= 1;
           
            }
       }
       break;

       case READ_DI: //读取开关量输入
       for ( tmp1=0; tmp1<*( source_p+2); tmp1++)
       {
            shift = 1;
            for (tmp2=0; tmp2<8; tmp2 ++)
            { 
              //同上，有问题？
              *(dest_p+data_start_address+tmp1*8+tmp2) = shift & *( source_p+3);
                 *( source_p+3)>>=1;
              
            }
       }
       break;

       case READ_HLD_REG:  //读取保持寄存器
       for ( tmp1=0; tmp1<*( source_p+2); tmp1+=2)
       {
            *(dest_p + data_start_address+ tmp1/2)= *( source_p+tmp1+3)*256 +  *( source_p+tmp1+4) ;
         
       }
       break ;

       case 4:      //读取模拟量输入
       for ( tmp1=0; tmp1<*( source_p+2); tmp1+=2)
       {
            *(dest_p + data_start_address+ tmp1/2) = *( source_p+tmp1+3)*256 +  *( source_p+tmp1+4) ;
        
       }
       break;

       case PROTOCOL_EXCEPTION:
       return -1*PROTOCOL_ERR;   
       //break;

       default:
       return -1*PROTOCOL_ERR;
       // break;
    }
    return 0;
}
            
