
/******************** (C) COPYRIGHT  ***************************************
 * �ļ���  ��uart1.c
 * ����    ��UART1�����ļ�   

*******************************************************************************/

/* ����ϵͳͷ�ļ� */

#include <stdarg.h>
#include "uart1.h"
#include <stdio.h> 
/* �Զ��������� */

/* �Զ���� */

/* ȫ�ֱ������� */

/*******************************************************************************
 * ����: Uart_Init
 * ����: UART1��ʼ������
 ******************************************************************************/
void Uart1_Init(void)
{
    UART1_DeInit();		/* ���Ĵ�����ֵ��λ */
	
	/*
	 * ��UART1����Ϊ��
	 * ������ = 115200
	 * ����λ = 8
	 * 1λֹͣλ
	 * ��У��λ
	 * ʹ�ܽ��պͷ���
	 * ʹ�ܽ����ж�
	 */
    UART1_DeInit();
    UART1_Init((u32)115200, UART1_WORDLENGTH_8D, UART1_STOPBITS_1, \
    UART1_PARITY_NO , UART1_SYNCMODE_CLOCK_DISABLE , UART1_MODE_TXRX_ENABLE);
    UART1_ITConfig(UART1_IT_RXNE_OR, ENABLE);//�������ݼĴ����ǿջ���������ж�
    //UART1_ITConfig(UART1_IT_RXNE, ENABLE);
    UART1_Cmd(ENABLE);
}

/*******************************************************************************
 * ����: UART1_SendByte
 * ����: UART1����һ���ֽ�
 * �β�: data -> Ҫ���͵��ֽ�
 * ����: ��
 * ˵��: �� 
 ******************************************************************************/
void UART1_SendByte(short data)
{
	UART1_SendData8((unsigned char)data);
	
	/* �ȴ�������� */
	while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET);
}


/************************************************************ 
 * ����:       USART0_Send_Data(u16 *buf, uint8_t len)
 * ˵��:       ���ڷ������ݺ���
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
 * ����: UART1_SendString
 * ����: UART1����len���ַ�
 * �β�: data -> ָ��Ҫ���͵��ַ���
 *       len -> Ҫ���͵��ֽ���
 * ����: ��
 * ˵��: �� 
 ******************************************************************************/
void UART1_SendString(u8* Data,u16 len)
{
	u16 i=0;
	for(; i < len; i++)
        UART1_SendByte(Data[i]);	/* ѭ�����÷���һ���ַ����� */
}

/*******************************************************************************
 * ����: UART1_ReceiveByte
 * ����: UART1����һ���ַ�
 * �β�: ��
 * ����: ���յ����ַ�
 * ˵��: �� 
 ******************************************************************************/
u8 UART1_ReceiveByte(void)
{
	u8 USART1_RX_BUF; 
	
	/* �ȴ�������� */
	while (UART1_GetFlagStatus(UART1_FLAG_RXNE) == RESET);
	
	USART1_RX_BUF = UART1_ReceiveData8();
	
	return  USART1_RX_BUF;
}


/*
  ���ܣ���int������תΪ2��8��10��16�����ַ���
  ������value --- �����int������
        str --- �洢ת�����ַ���
        radix --- ��������ѡ��
  ע�⣺8λ��Ƭ��int�ֽ�ֻռ2���ֽ�
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
    //ʮ������Ϊ����
    tmp_value = (unsigned int)(0 - value);
    str[i++] = '-';
    k = 1;
  } else 
  {
    tmp_value = (unsigned int)value;
  }
  
  //����ת��Ϊ�ַ���������洢
  do {
        str[i ++] = list[tmp_value%radix];
        tmp_value /= radix;
      } while(tmp_value);
  str[i] = '\0';
  //�������ַ���ת��Ϊ����
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
  ���ܣ���double������תΪ�ַ���
  ������value --- �����double������
        str --- �洢ת�����ַ���
        eps --- ����С��λѡ�����ٱ���һ��С��λ,���ౣ��4��С��λ
  ע�⣺8λ��Ƭ��int�ֽ�ֻռ2���ֽ�
*/
void sky_ftoa(double value, char *str, unsigned int eps)
{
  unsigned int integer;
  double decimal;
  char list[] = "0123456789";
  int i = 0, j, k = 0;
  //��������С��������ȡ����
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
  //������������ת��Ϊ�ַ���������洢
  do {
      str[i ++] = list[integer%10];
      integer /= 10;
     } while(integer);
  str[i] = '\0';
  //�������ַ���ת��Ϊ����
  char tmp;
  for (j = k; j < (i+k)/2; j++) 
  {
    tmp = str[j];
    str[j] = str[i-j-1+k];
    str[i-j-1+k] = tmp;
  }
  //����С������
  if (eps < 1 || eps > 4) 
  {
    eps = 4;
  }
  
  //�������⣬��ֹ����1.2���1.19�����
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
  //������������ת��Ϊ�ַ���������洢
  do {
    str[i ++] = list[tmp_decimal%10];
    tmp_decimal /= 10;
  } while(tmp_decimal);
  str[i] = '\0';
  //�������ַ���ת��Ϊ����
  for (j = k; j < (i+k)/2; j++) 
  {
    tmp = str[j];
    str[j] = str[i-j-1+k];
    str[i-j-1+k] = tmp;
  }
  str[i] = '\0';
}



/************************************************************************
 * ��������UART1_printf
 * ����  ����ʽ�������������C���е�printf��������û���õ�C��
 * ����  ��-UARTx ����ͨ��������ֻ�õ��˴���1����UART1
 *		     -Data   Ҫ���͵����ڵ����ݵ�ָ��
 *			   -...    ��������
 * ���  ����
 * ����  ���� 
 * ����  ���ⲿ����
 *         ����Ӧ��	 UART1_printf( "\r\n this is a demo \r\n" );
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