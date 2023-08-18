
// CS1237程序要注意的两个时序问题：
// 1.CLK拉低以后 要等待300ms OUT引脚才会自动变低
// 2.CLK的时钟频率最大是1.1Mhz 所以CLK=1和CLK=0之间 要加入至少0.5us的延时
/* Includes ------------------------------------------------------------------*/

#include "cs1237.h"

extern void delay_us(unsigned char n) ;
extern void delay_ms(unsigned int time);



#define Lv_Bo 0.00001       // 滤波系数 小于1
#define ADC_Bit  20        // ADC有效位数，带符号位 最高24位
u32 ADC_REG_WRITE=0;      // spi寄存器的配置状态 值
static long AD_Res_Last=0;//上一轮的ADC数值保存



//******************************************引脚初始化****************************************************
  void CS1237_GPIO_INIT(void)
    {
      GPIO_DeInit(GPIOD);
      
      GPIO_Init(CS1237_CLK_PORT, (GPIO_Pin_TypeDef)CS1237_CLK_PIN, GPIO_MODE_OUT_PP_HIGH_SLOW);   //初始化PD2   推挽输出
      GPIO_Init(CS1237_Data_PORT, (GPIO_Pin_TypeDef)CS1237_Data_PIN, GPIO_MODE_IN_PU_NO_IT);    //初始化PD3  推挽输出
      GPIO_Init(CS1237_VCC_PORT, (GPIO_Pin_TypeDef)CS1237_VCC_PIN, GPIO_MODE_OUT_PP_HIGH_SLOW); //初始化PD4   推挽输出

      GPIO_WriteLow(CS1237_CLK_PORT, (GPIO_Pin_TypeDef)CS1237_CLK_PIN);  //PD2拉高
      GPIO_WriteHigh(CS1237_Data_PORT, (GPIO_Pin_TypeDef)CS1237_Data_PIN);//PD3拉高
      GPIO_WriteHigh(CS1237_VCC_PORT, (GPIO_Pin_TypeDef)CS1237_VCC_PIN);  //PD4拉高
      
      GPIO_WriteHigh(CS1237_CLK_PORT, (GPIO_Pin_TypeDef)CS1237_CLK_PIN);  //PD2拉高
      delay_ms(500);
      GPIO_WriteLow(CS1237_CLK_PORT, (GPIO_Pin_TypeDef)CS1237_CLK_PIN);  //PD2拉高
      delay_ms(500);
    }
  
  
  //******************************************数据输出*****************************************************
  void CS1237_Data_OUT()
    {
       GPIO_Init(CS1237_Data_PORT, (GPIO_Pin_TypeDef)CS1237_Data_PIN, GPIO_MODE_OUT_PP_HIGH_SLOW);
    }

  //*****************************************数据输入引脚拉低*****************************************************
  void CS1237_Data_INPUT()
    {
       GPIO_Init(CS1237_Data_PORT, (GPIO_Pin_TypeDef)CS1237_Data_PIN, GPIO_MODE_IN_PU_NO_IT);
    }
//******************************************数据拉高*****************************************************
  void CS1237_Data_HIGH()
    {
        GPIO_WriteHigh(CS1237_Data_PORT, (GPIO_Pin_TypeDef)CS1237_Data_PIN);
    }


//*****************************************数据拉低******************************************************
  void CS1237_Data_LOW()
    {
       GPIO_WriteLow(CS1237_Data_PORT, (GPIO_Pin_TypeDef)CS1237_Data_PIN);
    }


//*******************************************时钟引脚拉高*******************************************************
  void CS1237_SCLK_HIGH()
    {
       GPIO_WriteHigh(CS1237_CLK_PORT, (GPIO_Pin_TypeDef)CS1237_CLK_PIN);  
    }


//*******************************************时钟引脚拉低********************************************************
  void CS1237_SCLK_LOW()
    {
       GPIO_WriteLow(CS1237_CLK_PORT, (GPIO_Pin_TypeDef)CS1237_CLK_PIN);    
    }
  
  
//*************************************************************************************************************
  void CS1237_VCC_HIGH()
    {
       GPIO_WriteHigh(CS1237_VCC_PORT, (GPIO_Pin_TypeDef)CS1237_VCC_PIN);    
    }


//********************************************一个时钟周期************************************************

// 10.41us
  
  void CS1237_OneClk()
    {
       CS1237_SCLK_HIGH();
       delay_us(1);
       CS1237_SCLK_LOW();
       delay_us(1);
    }



//********************************************进入低功耗模式****************************************************

  void CS1237_Power_Down(void)
    {
      CS1237_SCLK_HIGH();
      delay_us(100);
      CS1237_SCLK_HIGH();
      CS1237_SCLK_HIGH();
      delay_us(100);
    }



//*******************************************写SPI寄存器*************************************************
    void C1237_WriteReg(u8 mData)
      {
         u8 i = 0;
         unsigned int count_i=0;//溢出计时器
         u8 tem = 0xCA; // CA准备写命令(0x65)左移1位 下边开始命令写入 
                
         delay_us(50);//Wakeup MCU
         
         CS1237_Data_OUT();
         CS1237_Data_HIGH();
         CS1237_Data_INPUT();
         CS1237_SCLK_LOW();
         
         while (CS1237_Data_PIN_STATE())//需要实际工程使用需要加入超时 
          {
            delay_ms(1);
            count_i++;
            if(count_i > 300)
            {
               CS1237_Data_OUT();
               CS1237_Data_HIGH();; // OUT引脚拉高
               CS1237_SCLK_HIGH(); // CLK引脚拉高
               return;//超时，则直接退出程序
            }
          }
         
          for (i = 0; i < 26; i++) // 1~26th CLK
          {
              CS1237_OneClk();
          }
          //CS1237_Data_INPUT();
          //CS1237_Data_HIGH(); // force high
          CS1237_OneClk();    //27th CLK
                    
          //CS1237_Data_OUT();         
          CS1237_OneClk(); // 28th CLK
          CS1237_OneClk(); // 29th CLK
          CS1237_Data_OUT();     
                
          for (i = 0; i < 7; i++) // 30~36th CLK
            {  
              
             CS1237_SCLK_HIGH();
             delay_us(1);
             
              if (tem & 0x80)
              {
                CS1237_Data_HIGH();
               }
              else
              {
                CS1237_Data_LOW();
               }
              
              CS1237_SCLK_LOW();
              delay_us(1);
              
              tem <<= 1;
            }   
          
         // CS1237_Data_OUT();  // direction chage to intput direction
          CS1237_OneClk();            // 37th CLK
          
          for (i = 0; i < 8; i++)     // 38~45th CLK
            {  
             CS1237_SCLK_HIGH();
             delay_us(1);
             
              if (mData & 0x80)
              {
                CS1237_Data_HIGH();
               }
              else
              {
                CS1237_Data_LOW();
               } 
              
              CS1237_SCLK_LOW();
              delay_us(1);
              
              mData <<= 1;
            }
          
          CS1237_Data_INPUT();
          //CS1237_Data_HIGH();
          CS1237_OneClk(); // 46th CLK     
          //CS1237_Data_INPUT();
      
}



//********************************************读SPI寄存器************************************************
  
u8 CS1237_ReadReg()
      { 
         u8 i = 0;
         u8 tem = 0xAC;     // AC准备读命令(0x56)左移1位 下边开始命令写入
         unsigned int count_i=0;//溢出计时器
         u8 con =0;         

         delay_us(50);//Wakeup MCU  
        
         CS1237_Data_OUT();
         CS1237_Data_HIGH();
         CS1237_Data_INPUT();
         CS1237_SCLK_LOW();
         
         while (CS1237_Data_PIN_STATE())//需要实际工程使用需要加入超时 
          {
            delay_ms(1);
            count_i++;
            if(count_i > 300)
            {
               CS1237_Data_OUT();
               CS1237_Data_HIGH();; // OUT引脚拉高
               CS1237_SCLK_HIGH(); // CLK引脚拉高
               return 1;//超时，则直接退出程序
            }
          }
           
          for (i = 0; i < 26; i++) // 1-26th CLK   前24读ADC 数据 后25 26读寄存器写操作状态         
          {
            CS1237_OneClk();
          }

          //CS1237_Data_HIGH(); // Force High  cs1237 direction input  27 那根线输出线拉高 
          CS1237_OneClk();    // 27th CLK  
                   
          
          CS1237_Data_OUT();
          CS1237_OneClk(); // 28th CLK   
          CS1237_OneClk(); // 29th CLK  那根线切换成输入 
          //CS1237_Data_OUT();                             
          for (i = 0; i < 7; i++) // 30~36th CLK  开始高位先输入
            {  
             CS1237_SCLK_HIGH();
             delay_us(1);  
             
              if (tem & 0x80)
              {
               CS1237_Data_HIGH();
               }
              else
              {
               CS1237_Data_LOW();
               } 
              
              CS1237_SCLK_LOW();
              delay_us(1);                           
              tem <<= 1;
            }
   
          CS1237_Data_INPUT();     // cs1237 change to output direction切换那根线的方向
         // CS1237_Data_LOW();          
          CS1237_OneClk();         // 37th CLK
       
          for (i = 0; i < 8; i++)  // 38~45th CLK  输入寄存器配置数据或输出寄存器配置数据  高位先
            {
              con <<= 1;
              CS1237_OneClk();
              if (CS1237_Data_PIN_STATE())
              con++;
            }
          
          //CS1237_Data_INPUT();
         // CS1237_Data_HIGH();
          CS1237_OneClk(); // 46th CLK   
         // CS1237_Data_INPUT();
          return con;

      }




//********************************************AD芯片初始化****************************************************

    void C1237_Init(unsigned char refOutable, C1237_SPEED msel, C1237_PGA mselPga, C1237_CHN mselChn)
      {
        volatile u8 config = 0;                  
        config= ((refOutable&0x01)<<6)|((msel&0x03)<<4)|((mselPga&0x03)<<2)|((mselChn&0x03)<<0);
        C1237_WriteReg(config);
        ADC_REG_WRITE = config; // 芯片的配置信息 0C 初始化
     
      }


//*******************************读取24位AD值并抛去无效位***********************************
  double C1237_ReadResult()
    {
        u8 i=0;
        u8 j=0;
        unsigned char count_i=0;//溢出计时器
        int32_t temp=0;
        uint32_t rdata=0; //读取到的数据 
        
         CS1237_Data_OUT();
         CS1237_Data_HIGH();
         CS1237_Data_INPUT();
         CS1237_SCLK_LOW();
         
         while (CS1237_Data_PIN_STATE())//需要实际工程使用需要加入超时 
          {
            delay_ms(1);
            count_i++;
            if(count_i > 300)
            {
               CS1237_Data_OUT();
               CS1237_Data_HIGH();; // OUT引脚拉高
               CS1237_SCLK_HIGH(); // CLK引脚拉高
               return 1;//超时，则直接退出程序
            }
          }
         
        for (i = 0; i < 24; i++)
        {
         CS1237_SCLK_HIGH();
         delay_us(1);
       
          rdata <<= 1;           
          if (CS1237_Data_PIN_STATE()) // read data  
            rdata++;
          
         CS1237_SCLK_LOW();
         delay_us(1);             
        }
        
         CS1237_OneClk();
         CS1237_OneClk();
         CS1237_OneClk();   //27th CLK
        
        
	//如果一个数据转换周期内对ADC进行两次或以上的读写操作，则需要发送46个CLK，而不是27个CLK
	//(即：发送24个CLK读取数据，然后再发送22个CLK)        
       //  for (j = 0; j < 22; j++)
       //   {

       //     CS1237_OneClk();

       //   }
        //CS1237_Data_OUT();
        //CS1237_Data_HIGH();     
        
       j = 24 - ADC_Bit;//i表示将要丢弃的位数
       temp >>= j;     //丢弃多余的位数
        
	if(rdata&0x00800000)// 判断是负数 最高位24位是符号位
	{
          temp=-(((~rdata)&0x007FFFFF) + 1);// 补码变源码
	}
	else
	  temp=rdata; // 正数的补码就是源码



        return temp;

    }

 
//********************************************读取滤波后的实际值************************************************

//计算公式：Y=a*x+(1-a)*Y_last

//Y是本次输出，x是本次输入，Y_last是上一次输出
//a是滤波系数，1-a越大，惯性(滞后)越大，说明本次的输出和上一次的输出关系越大
//数字一阶滤波 滤波系数A，小于1。上一次数值B，本次数值C out = b*A + C*(1-A)
//下面这个程序负责读取出最终ADC数据
long C1237_Read20BitAD(void)
  {
    long out,c;
    
    out = AD_Res_Last;
    c = C1237_ReadResult();
    if(c!=0)     // 读到正确数据
    {
      out = out*Lv_Bo + c*(1-Lv_Bo);
      AD_Res_Last = out;//把这次的计算结果放到全局变量里面保护
    }
    return AD_Res_Last;
  }

/************************************************************ 
 * 函数:        Vol_convert_Elec(float vol)
 * 说明:        将通道电压转换为4-20ma电流 小数输出  
************************************************************/
		
  double Vol_convert_Elec(double vol)
    {
      double elec;
      elec=(((vol-1)*16)/4)+4;
      return(elec);
    }

  
/***********************************************************************
函数功能：将小数转换成IEEE754数据类型，用于modbus协议传输数据
函数参数：value 小数值    Holdaddr 从机保持寄存器地址
***********************************************************************/		
 void Float_To_U32(float value, uint8_t *buf)
    {
        un_DtformConver Data;
        u16 Reg[2];//保持寄存器 
        u16 us_temp = 0;
        Data.ul_Temp = value;    //获取小数值

       //协议定义高位在前
        Reg[0] = Data.uc_Buf[0];//获取16~23位 
        us_temp = Data.uc_Buf[1];        //获取24~31位
        Reg[0] |= us_temp <<8;  //高16位保存到保持寄存器中
        
        buf[1] = Reg[0] >> 8;
	buf[0] = Reg[0];


        Reg[1] = Data.uc_Buf[2];  //获取最低8位
        us_temp = Data.uc_Buf[3];        //获取8~15位
        Reg[1] |= us_temp <<8;    //低16位保存到保持寄存器中  
        
        buf[3] = Reg[1] >> 8;
	buf[2] = Reg[1];

    }




  
  
  
  
 