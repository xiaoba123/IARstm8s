/**
  ******************************************************************************
  * @file    Project/main.c 
  * @author  MCD Application Team
  * @version V2.1.0
  * @date    18-November-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 


/* Includes ------------------------------------------------------------------*/
/* 包含系统头文件 */
    
#include "stm8s.h"
#include "cs1237.h"
#include "uart1.h"
#include "timer1.h"
#include "wwdg.h"


/* Private defines -----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

void delay_us(unsigned char n) ;
void delay_ms(unsigned int time);

short timerflag=0;
u32 ADC_REG_READ=0;   // 读取到的寄存器状态值
extern u32 ADC_REG_WRITE;      // spi寄存器的配置状态 值
long ADC_24RES=0;     // 存储读取到的ADC值
long ADC_20RES=0;     // 存储读取到的ADC值

u8 data[4]={0};
u8 i=0;
double a=0,b=0;

void main(void)
{
  /* Infinite loop */
  /*设置内部时钟16M为主时钟*/  

  CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);
  delay_us(100);
  sim();             //disable interrupt
  Tim1_Init();
  Uart1_Init();

  CS1237_GPIO_INIT();  // CS1237 引脚初始化
  C1237_Init(1,C1237_10HZ,C1237_PGA_2,C1237_CHA);// 写入寄存器配置 0C 复位
  delay_ms(100);
  do{ADC_REG_READ=CS1237_ReadReg();delay_us(100);}
  while(ADC_REG_READ!=ADC_REG_WRITE);	
  
  WWDG_Configuration(); //配置使用软件喂狗时间为5.376ms ~ 46.848ms
  rim();              //enable interrupt
  
  while (1)
  {    
        Refresh_WWDG_Window();
    // ADC的分辨率=测量电压范围/(2^AD位数-1)，精度取决于LSB失调误差与增益误差。
      //delay_ms(100);
      //ADC_24RES=C1237_ReadResult();
      //if(ADC_24RES<0)ADC_24RES=0;
      //a=(ADC_24RES*((0.5*3.3)/2))/8388607;    
      ADC_20RES=C1237_Read20BitAD();
      Refresh_WWDG_Window();
      CS1237_Power_Down();

      if(ADC_20RES<0)ADC_20RES=0;
      if(ADC_20RES==0)b=0.0;
      if(ADC_20RES<=208500)b=((ADC_20RES+22220)/1.0018)/10000000;   //小于20mv
      if((ADC_20RES>208500)&&(ADC_20RES<8388607))b=(((ADC_20RES+24768)/1.0127)/10000000)-0.0029;   // 20mv-825mv      
      if(ADC_20RES>=8388607)b=0.825;
      //Float_To_U32(a,data);

      if(timerflag==1)
      {
        mprintf("b=%f",b);   
        timerflag=0;
      }
      
      i++;
      if(i>500)//大概23秒后复位CPU
      {
        i=0;
        WWDG_SWReset();//窗口看门狗令CPU复位,Activate WWDG, with clearing T6
      }
        
      
      //UART1_Send_Data(data,4);

  }
  
}

  


//---- 微妙级延时程序-----------------------  y = 0.3739x + 5.1713

  void delay_us(unsigned char n) 
    {
 
     u8 t=0;
     for(t=0;t<n;t++)
      {
       asm("nop"); 
       asm("nop"); 
      }
    }

//---- 毫秒级延时程序-----------------------
  void delay_ms(unsigned int time)
    {
      unsigned int i;
      while(time--)
      for(i=900;i>0;i--)
      delay_us(1);
    }




#ifdef USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *   where the assert_param error has occurred.
  * @param file: pointer to the source file name
  * @param line: assert_param error line source number
  * @retval : None
  */
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
