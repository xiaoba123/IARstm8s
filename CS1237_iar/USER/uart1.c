
/******************** (C) COPYRIGHT  ***************************************
 * 文件名  ：uart1.c
 * 描述    ：UART1设置文件   

*******************************************************************************/

/* 包含系统头文件 */

#include <stdarg.h>
#include "uart1.h"
#include <stdio.h> 
/* 自定义新类型 */

/* 自定义宏 */

/* 全局变量定义 */

/*******************************************************************************
 * 名称: Uart_Init
 * 功能: UART1初始化操作
 ******************************************************************************/
void Uart1_Init(void)
{
    UART1_DeInit();		/* 将寄存器的值复位 */
	
	/*
	 * 将UART1配置为：
	 * 波特率 = 115200
	 * 数据位 = 8
	 * 1位停止位
	 * 无校验位
	 * 使能接收和发送
	 * 使能接收中断
	 */
    UART1_DeInit();
    UART1_Init((u32)115200, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, \
    UART1_PARITY_NO , UART1_SYNCMODE_CLOCK_DISABLE , UART1_MODE_TXRX_ENABLE);
    UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);//接收数据寄存器非空或溢出错误中断
    //UART1_ITConfig(UART1_IT_RXNE, ENABLE);
    UART1_Cmd(ENABLE);
}

/*******************************************************************************
 * 名称: UART1_SendByte
 * 功能: UART1发送一个字节
 * 形参: data -> 要发送的字节
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void UART1_SendByte(short data)
{
	UART1_SendData8((unsigned char)data);
	
	/* 等待传输结束 */
	while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);
}


/************************************************************ 
 * 函数:       USART0_Send_Data(u16 *buf, uint8_t len)
 * 说明:       串口发送数据函数
************************************************************/	 
		
void UART1_Send_Data(uint8_t *buf, uint8_t len)
{
       uint8_t t;
       for(t=0;t<len;t++)      
       {           
          UART1_SendByte(buf[t]);
       }    
}



/*******************************************************************************
 * 名称: UART1_SendString
 * 功能: UART1发送len个字符
 * 形参: data -> 指向要发送的字符串
 *       len -> 要发送的字节数
 * 返回: 无
 * 说明: 无 
 ******************************************************************************/
void UART1_SendString(u8* Data,u16 len)
{
	u16 i=0;
	for(; i < len; i++)
        UART1_SendByte(Data[i]);	/* 循环调用发送一个字符函数 */
}

/*******************************************************************************
 * 名称: UART1_ReceiveByte
 * 功能: UART1接收一个字符
 * 形参: 无
 * 返回: 接收到的字符
 * 说明: 无 
 ******************************************************************************/
u8 UART1_ReceiveByte(void)
{
	u8 USART1_RX_BUF; 
	
	/* 等待接收完成 */
	while (UART1_GetFlagStatus(UART1_FLAG_RXNE) == RESET);
	
	USART1_RX_BUF = UART1_ReceiveData8();
	
	return  USART1_RX_BUF;
}


/*
  功能：将int型数据转为2，8，10，16进制字符串
  参数：value --- 输入的int整型数
        str --- 存储转换的字符串
        radix --- 进制类型选择
  注意：8位单片机int字节只占2个字节
*/
char *sky_itoa(int value, char *str, unsigned int radix)
{
  char list[] = "0123456789ABCDEF";
  unsigned int tmp_value;
  int i = 0, j, k = 0;
  if (NULL == str) {
    return NULL;
  }
  if (2 != radix && 8 != radix && 10 != radix && 16 != radix) 
  {
    return NULL;
  }
  if (radix == 10 && value < 0)
  {
    //十进制且为负数
    tmp_value = (unsigned int)(0 - value);
    str[i++] = '-';
    k = 1;
  } else 
  {
    tmp_value = (unsigned int)value;
  }
  
  //数据转换为字符串，逆序存储
  do {
        str[i ++] = list[tmp_value%radix];
        tmp_value /= radix;
      } while(tmp_value);
  str[i] = '\0';
  //将逆序字符串转换为正序
  char tmp;
  for (j = k; j < (i+k)/2; j++) 
  {
    tmp = str[j];
    str[j] = str[i-j-1+k];
    str[i-j-1+k] = tmp;
  }
  return str;
}


/*
  功能：将double型数据转为字符串
  参数：value --- 输入的double浮点数
        str --- 存储转换的字符串
        eps --- 保留小数位选择，至少保留一个小数位,至多保留4个小数位
  注意：8位单片机int字节只占2个字节
*/
void sky_ftoa(double value, char *str, unsigned int eps)
{
  unsigned int integer;
  double decimal;
  char list[] = "0123456789";
  int i = 0, j, k = 0;
  //将整数及小数部分提取出来
  if (value < 0)
  {
    decimal = (double)(((int)value) - value);
    integer = (unsigned int)(0 - value);
    str[i ++] = '-';
    k = 1;
  } 
  else 
  {
    integer = (unsigned int)(value);
    decimal = (double)(value - integer);
  }
  //整数部分数据转换为字符串，逆序存储
  do {
      str[i ++] = list[integer%10];
      integer /= 10;
     } while(integer);
  str[i] = '\0';
  //将逆序字符串转换为正序
  char tmp;
  for (j = k; j < (i+k)/2; j++) 
  {
    tmp = str[j];
    str[j] = str[i-j-1+k];
    str[i-j-1+k] = tmp;
  }
  //处理小数部分
  if (eps < 1 || eps > 4) 
  {
    eps = 4;
  }
  
  //精度问题，防止输入1.2输出1.19等情况
  double pp = 0.1;
  for (j = 0; j <= eps; j++) 
  {
    pp *= 0.1;
  }
  decimal += pp;
  while (eps) 
  {
    decimal *= 10;
    eps --;
  }
  int tmp_decimal = (int)decimal;
  str[i ++] = '.';
  k = i;
  //整数部分数据转换为字符串，逆序存储
  do {
    str[i ++] = list[tmp_decimal%10];
    tmp_decimal /= 10;
  } while(tmp_decimal);
  str[i] = '\0';
  //将逆序字符串转换为正序
  for (j = k; j < (i+k)/2; j++) 
  {
    tmp = str[j];
    str[j] = str[i-j-1+k];
    str[i-j-1+k] = tmp;
  }
  str[i] = '\0';
}



/************************************************************************
 * 函数名：UART1_printf
 * 描述  ：格式化输出，类似于C库中的printf，但这里没有用到C库
 * 输入  ：-UARTx 串口通道，这里只用到了串口1，即UART1
 *		     -Data   要发送到串口的内容的指针
 *			   -...    其他参数
 * 输出  ：无
 * 返回  ：无 
 * 调用  ：外部调用
 *         典型应用	 UART1_printf( "\r\n this is a demo \r\n" );
 *            		 UART1_printf(  "\r\n %d \r\n", i );
 *            		 UART1_printf(  "\r\n %s \r\n", j );
 ***************************************************************************/

void mprintf(char * Data, ...)
{
  const char *s;
  int d;   
  char buf[16];
  uint8_t txdata;
  va_list ap;
  va_start(ap, Data);
  while ( * Data != 0 ) 
  {				                          
    if ( * Data == 0x5c )  
    {									  
      switch ( *++Data ) 
      {
        case 'r':							          
          txdata = 0x0d;
          UART1_SendByte(txdata);
          Data ++;
          break;
        case 'n':							          
          txdata = 0x0a;
          UART1_SendByte(txdata);
          Data ++;
          break;
        default:
          Data ++;
          break;
      }			 
    } else if ( * Data == '%') 
      {									  
        switch ( *++Data ) 
        {				
        case 's':										 
          s = va_arg(ap, const char *);
          for ( ; *s; s++) 
            {
              UART1_SendByte(*((uint8_t *)s));
            }				
          Data++;				
          break;
        case 'd':			
          d = va_arg(ap, int);					
          sky_itoa(d, buf, 10);					
          for (s = buf; *s; s++) 
            {
              UART1_SendByte(*((uint8_t *)s));
            }					
          Data++;				
          break;
        case 'x': 
          {
            d = va_arg(ap, int);					
            sky_itoa(d, buf, 16);					
            for (s = buf; *s; s++) {
            UART1_SendByte(*((uint8_t *)s));
          }					
          Data++;			
          break;
        }
        case 'f': 
          {
          double num = va_arg(ap, double);					
          sky_ftoa(num, buf, 4);
          for (s = buf; *s; s++) 
            {
              UART1_SendByte(*((uint8_t *)s));
            }					
          Data++;			
          break;
        }
        default:
          Data++;				
          break;				
        }		 
      } else 
        {
            UART1_SendByte(*((uint8_t *)Data));
            Data++;
        }
  }
}

/********************************END OF FILE******************************/