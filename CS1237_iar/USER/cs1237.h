#ifndef __CS1237_H__
#define __CS1237_H__


/* Includes ------------------------------------------------------------------*/


#include "stm8s.h"


#define  CS1237_CLK_PORT    (GPIOD)
#define  CS1237_CLK_PIN     (GPIO_PIN_2)

#define  CS1237_Data_PORT   (GPIOD)
#define  CS1237_Data_PIN    (GPIO_PIN_3)
#define  CS1237_Data_PIN_STATE()   GPIO_ReadInputPin(CS1237_Data_PORT,(GPIO_Pin_TypeDef)CS1237_Data_PIN)

#define  CS1237_VCC_PORT    (GPIOD)
#define  CS1237_VCC_PIN     (GPIO_PIN_4)




typedef union                                        
{
   float ul_Temp;
   uint8_t  uc_Buf[4];  //小数转换成IEEE754格式的数组
}un_DtformConver;



//************************************************************************************************************
typedef enum
{
	C1237_10HZ = 0,
	C1237_40HZ = 1,
	C1237_64HZ = 2,
	C1237_128HZ = 3
}C1237_SPEED;
//************************************************************************************************************
typedef enum
{
	C1237_PGA_1 = 0,
	C1237_PGA_2 = 1,
	C1237_PGA_64 = 2,
	C1237_PGA_128 = 3
}C1237_PGA;
//************************************************************************************************************
typedef enum
{
	C1237_CHA = 0, //adc channel
	C1237_RST = 1, //cheap reserved
	C1237_TEM = 2, //temperature test
	C1237_INS = 3 //internal short
}C1237_CHN;




void CS1237_GPIO_INIT(void);

/************************************************************************************************************
Function: C1237_OneClk();
Return value: void
Discription: C1237 input a clk pusle for to read or write
Example:
  C1237_OneClk();
*************************************************************************************************************/
void C1237_OneClk();

/************************************************************************************************************
Function: CS1237_ReadReg();
Return value: u8
Discription: read CS1237 register
Example:
  CS1237_ReadReg();
*************************************************************************************************************/
u8 CS1237_ReadReg();

/************************************************************************************************************
Function: C1237_WriteReg(u8 mData);
Return value: void
Discription: wreite data to C1237 register
Example:
  C1237_WriteReg(mData);
*************************************************************************************************************/
void C1237_WriteReg(u8 mData);
 
/************************************************************************************************************
Function: C1237_Init(BOOL refOutable,C1237_SPEED msel,C1237_PGA mselPga,C1237_CHN mselChn);
Return value: void
Discription: CS1237 init
Example:
  C1237_Init(0,C1237_10HZ,C1237_PGA_1,C1237_CHA);//ref voltage out disable,speed 10Hz,Pga 1, work at Adc mode
*************************************************************************************************************/
void C1237_Init(unsigned char refOutable, C1237_SPEED msel, C1237_PGA mselPga, C1237_CHN mselChn);

/************************************************************************************************************
Function: C1237_ReadResult();
Return value: u32
Discription: read C1237 A/D result
Example:
  C1237_ReadResult();
*************************************************************************************************************/
double C1237_ReadResult();
void CS1237_Power_Down(void);
long C1237_Read20BitAD(void);
void Float_To_U32(float value, uint8_t *buf);
 
#endif