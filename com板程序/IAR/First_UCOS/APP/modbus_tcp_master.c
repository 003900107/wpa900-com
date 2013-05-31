#include "includes.h"
#include "modbus_tcp_master.h"

#define READ_COIL     0x01
#define READ_DI       0x02
#define READ_HLD_REG  0x03
#define READ_AI       0x04
#define SET_COIL      0x05
#define SET_HLD_REG   0x06
#define READ_FIFO     24
#define PROTOCOL_EXCEPTION 0x81
#define PROTOCOL_ERR  1
#define FRM_ERR       2

static INT8U HI(INT16U data)
{
  return (INT8U)((data>>8)&0xFF);
}

static INT8U LOW(INT16U data)
{
  return (INT8U)(data&0xFF);
}

void construct_tcp_frm(unsigned char *dst_buf,unsigned char *src_buf,unsigned char lenth)
{
  while(lenth--)
  {
    *dst_buf = *src_buf ;
    dst_buf++;
    src_buf++;
  }
}

//读线圈：01  "DO"
INT16U tcp_getCoilStatus(INT8U addr,INT8U *buf,INT16U start_address,INT16U lenth) 
{
  unsigned char tmp[256], tmp_lenth;
    
  tmp[0]  = 0x00;
  tmp[1]  = 0x00;
  tmp[2]  = 0x00;
  tmp[3]  = 0x00;
  tmp[4]  = 0x00;
  tmp[5]  = 0x06;
  tmp[6]  = addr;
  tmp[7]  = READ_COIL;
  tmp[8]  = HI(start_address);
  tmp[9]  = LOW(start_address);
  tmp[10] = HI(lenth);
  tmp[11] = LOW(lenth);
  tmp_lenth = 12 ;
  
  construct_tcp_frm(buf,tmp,tmp_lenth);
  return 12;
}

//读离散量输入：02  "DI"
INT16U tcp_getInputStatus(INT8U addr,INT8U *buf,INT16U start_address,INT16U lenth) 
{
  unsigned char tmp[256], tmp_lenth;
    
  tmp[0]  = 0;
  tmp[1]  = 0;
  tmp[2]  = 0;
  tmp[3]  = 0;
  tmp[4]  = 0;
  tmp[5]  = 6;
  tmp[6]  = addr;
  tmp[7]  = READ_DI;
  tmp[8]  = HI(start_address);
  tmp[9]  = LOW(start_address);
  tmp[10] = HI(lenth);
  tmp[11] = LOW(lenth);
  tmp_lenth = 12 ;
  
  construct_tcp_frm(buf,tmp,tmp_lenth);
  return 12;
}

//读保持寄存器：03  "AO"
INT16U  tcp_getHoldingRegisters(INT8U addr,INT8U *buf,INT16U start_address,INT16U lenth) 
{
  unsigned char tmp[256], tmp_lenth;
    
  tmp[0]  = 0;
  tmp[1]  = 0;
  tmp[2]  = 0;
  tmp[3]  = 0;
  tmp[4]  = 0;
  tmp[5]  = 6;
  tmp[6]  = addr;
  tmp[7]  = READ_HLD_REG;
  tmp[8]  = HI(start_address);
  tmp[9]  = LOW(start_address);
  tmp[10] = HI(lenth);
  tmp[11] = LOW(lenth);
  tmp_lenth = 12 ;
  
  construct_tcp_frm(buf,tmp,tmp_lenth);
  return 12;
}

//读输入寄存器:04  "AI"
INT16U tcp_getAnalogInput(INT8U addr,INT8U *buf,INT16U start_address,INT16U lenth) 
{
  unsigned char tmp[256], tmp_lenth;
    
  tmp[0]  = 0;
  tmp[1]  = 0;
  tmp[2]  = 0;
  tmp[3]  = 0;
  tmp[4]  = 0;
  tmp[5]  = 6;
  tmp[6]  = addr;
  tmp[7]  = READ_AI;
  tmp[8]  = HI(start_address);
  tmp[9]  = LOW(start_address);
  tmp[10] = HI(lenth);
  tmp[11] = LOW(lenth);
  tmp_lenth = 12 ;
  
  construct_tcp_frm(buf,tmp,tmp_lenth);
  return 12;
}

//写单个线圈:05  "DO"
INT16U  tcp_setCoil(INT8U addr,INT8U *buf,INT16U start_address,INT16U status)
{
  unsigned char tmp[256], tmp_lenth;
    
  tmp[0]  = 0;
  tmp[1]  = 0;
  tmp[2]  = 0;
  tmp[3]  = 0;
  tmp[4]  = 0;
  tmp[5]  = 6;
  tmp[6]  = addr;
  tmp[7]  = SET_COIL;
  tmp[8]  = HI(start_address);
  tmp[9]  = LOW(start_address);
  
  
  //tmp[10] = HI(status);
  //tmp[11] = LOW(status);
  if ( status )
  {
    //tmp[10] = 0xff ;
    //tmp[11] = 0x00 ;
    tmp[10] = 0x00 ;
    tmp[11] = 0xcc ;
  }
  else
  {
    //tmp[10] = 0x00 ;
    //tmp[11] = 0x00 ;
    tmp[10] = 0x00 ;
    tmp[11] = 0xaa ;
  }
  
  tmp_lenth = 12 ;
  
  construct_tcp_frm(buf,tmp,tmp_lenth);
  return 12;
}

//写单个寄存器:06
INT16U tcp_setHoldingRegister(INT8U addr,INT8U *buf, INT16U start_address,INT16U value )
{
  unsigned char tmp[256], tmp_lenth;
    
  tmp[0]  = 0;
  tmp[1]  = 0;
  tmp[2]  = 0;
  tmp[3]  = 0;
  tmp[4]  = 0;
  tmp[5]  = 6;
  tmp[6]  = addr;
  tmp[7]  = SET_HLD_REG;
  tmp[8]  = HI(start_address);
  tmp[9]  = LOW(start_address);
  tmp[10] = HI(value);
  tmp[11] = LOW(value);
  tmp_lenth = 12 ;
  
  construct_tcp_frm(buf,tmp,tmp_lenth);
  return 12;
}

//接收分析
INT16U tcp_data_anlys( int  *dest_p,INT8U *source_p,INT16U data_start_address, INT16U fr_lenth)
{
  unsigned char tmp1, tmp2, shift;
  
  switch ( *(source_p+7) ) // 功能码
    {
       case READ_COIL:                   //读取继电器状态 
       for ( tmp1=0; tmp1<*( source_p+8); tmp1++)
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
       for ( tmp1=0; tmp1<*( source_p+8); tmp1++)
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
       for ( tmp1=0; tmp1<*( source_p+8); tmp1+=2)
       {
            *(dest_p + data_start_address+ tmp1/2)= *( source_p+tmp1+9)*256 +  *( source_p+tmp1+10) ;
         
       }
       break ;
       
       case 4:      //读取模拟量输入
       for ( tmp1=0; tmp1<*( source_p+8); tmp1+=2)
       {
            *(dest_p + data_start_address+ tmp1/2) = *( source_p+tmp1+9)*256 +  *( source_p+tmp1+10) ;
        
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
