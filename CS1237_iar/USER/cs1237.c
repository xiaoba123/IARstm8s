
// CS1237����Ҫע�������ʱ�����⣺
// 1.CLK�����Ժ� Ҫ�ȴ�300ms OUT���ŲŻ��Զ����
// 2.CLK��ʱ��Ƶ�������1.1Mhz ����CLK=1��CLK=0֮�� Ҫ��������0.5us����ʱ
/* Includes ------------------------------------------------------------------*/

#include "cs1237.h"

extern void delay_us(unsigned char n) ;
extern void delay_ms(unsigned int time);



#define Lv_Bo 0.00001       // �˲�ϵ�� С��1
#define ADC_Bit  20        // ADC��Чλ����������λ ���24λ
u32 ADC_REG_WRITE=0;      // spi�Ĵ���������״̬ ֵ
static long AD_Res_Last=0;//��һ�ֵ�ADC��ֵ����



//******************************************���ų�ʼ��****************************************************
  void CS1237_GPIO_INIT(void)
    {
      GPIO_DeInit(GPIOD);
      
      GPIO_Init(CS1237_CLK_PORT, (GPIO_Pin_TypeDef)CS1237_CLK_PIN, GPIO_MODE_OUT_PP_HIGH_SLOW);   //��ʼ��PD2   �������
      GPIO_Init(CS1237_Data_PORT, (GPIO_Pin_TypeDef)CS1237_Data_PIN, GPIO_MODE_IN_PU_NO_IT);    //��ʼ��PD3  �������
      GPIO_Init(CS1237_VCC_PORT, (GPIO_Pin_TypeDef)CS1237_VCC_PIN, GPIO_MODE_OUT_PP_HIGH_SLOW); //��ʼ��PD4   �������

      GPIO_WriteLow(CS1237_CLK_PORT, (GPIO_Pin_TypeDef)CS1237_CLK_PIN);  //PD2����
      GPIO_WriteHigh(CS1237_Data_PORT, (GPIO_Pin_TypeDef)CS1237_Data_PIN);//PD3����
      GPIO_WriteHigh(CS1237_VCC_PORT, (GPIO_Pin_TypeDef)CS1237_VCC_PIN);  //PD4����
      
      GPIO_WriteHigh(CS1237_CLK_PORT, (GPIO_Pin_TypeDef)CS1237_CLK_PIN);  //PD2����
      delay_ms(500);
      GPIO_WriteLow(CS1237_CLK_PORT, (GPIO_Pin_TypeDef)CS1237_CLK_PIN);  //PD2����
      delay_ms(500);
    }
  
  
  //******************************************�������*****************************************************
  void CS1237_Data_OUT()
    {
       GPIO_Init(CS1237_Data_PORT, (GPIO_Pin_TypeDef)CS1237_Data_PIN, GPIO_MODE_OUT_PP_HIGH_SLOW);
    }

  //*****************************************����������������*****************************************************
  void CS1237_Data_INPUT()
    {
       GPIO_Init(CS1237_Data_PORT, (GPIO_Pin_TypeDef)CS1237_Data_PIN, GPIO_MODE_IN_PU_NO_IT);
    }
//******************************************��������*****************************************************
  void CS1237_Data_HIGH()
    {
        GPIO_WriteHigh(CS1237_Data_PORT, (GPIO_Pin_TypeDef)CS1237_Data_PIN);
    }


//*****************************************��������******************************************************
  void CS1237_Data_LOW()
    {
       GPIO_WriteLow(CS1237_Data_PORT, (GPIO_Pin_TypeDef)CS1237_Data_PIN);
    }


//*******************************************ʱ����������*******************************************************
  void CS1237_SCLK_HIGH()
    {
       GPIO_WriteHigh(CS1237_CLK_PORT, (GPIO_Pin_TypeDef)CS1237_CLK_PIN);  
    }


//*******************************************ʱ����������********************************************************
  void CS1237_SCLK_LOW()
    {
       GPIO_WriteLow(CS1237_CLK_PORT, (GPIO_Pin_TypeDef)CS1237_CLK_PIN);    
    }
  
  
//*************************************************************************************************************
  void CS1237_VCC_HIGH()
    {
       GPIO_WriteHigh(CS1237_VCC_PORT, (GPIO_Pin_TypeDef)CS1237_VCC_PIN);    
    }


//********************************************һ��ʱ������************************************************

// 10.41us
  
  void CS1237_OneClk()
    {
       CS1237_SCLK_HIGH();
       delay_us(1);
       CS1237_SCLK_LOW();
       delay_us(1);
    }



//********************************************����͹���ģʽ****************************************************

  void CS1237_Power_Down(void)
    {
      CS1237_SCLK_HIGH();
      delay_us(100);
      CS1237_SCLK_HIGH();
      CS1237_SCLK_HIGH();
      delay_us(100);
    }



//*******************************************дSPI�Ĵ���*************************************************
    void C1237_WriteReg(u8 mData)
      {
         u8 i = 0;
         unsigned int count_i=0;//�����ʱ��
         u8 tem = 0xCA; // CA׼��д����(0x65)����1λ �±߿�ʼ����д�� 
                
         delay_us(50);//Wakeup MCU
         
         CS1237_Data_OUT();
         CS1237_Data_HIGH();
         CS1237_Data_INPUT();
         CS1237_SCLK_LOW();
         
         while (CS1237_Data_PIN_STATE())//��Ҫʵ�ʹ���ʹ����Ҫ���볬ʱ 
          {
            delay_ms(1);
            count_i++;
            if(count_i > 300)
            {
               CS1237_Data_OUT();
               CS1237_Data_HIGH();; // OUT��������
               CS1237_SCLK_HIGH(); // CLK��������
               return;//��ʱ����ֱ���˳�����
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



//********************************************��SPI�Ĵ���************************************************
  
u8 CS1237_ReadReg()
      { 
         u8 i = 0;
         u8 tem = 0xAC;     // AC׼��������(0x56)����1λ �±߿�ʼ����д��
         unsigned int count_i=0;//�����ʱ��
         u8 con =0;         

         delay_us(50);//Wakeup MCU  
        
         CS1237_Data_OUT();
         CS1237_Data_HIGH();
         CS1237_Data_INPUT();
         CS1237_SCLK_LOW();
         
         while (CS1237_Data_PIN_STATE())//��Ҫʵ�ʹ���ʹ����Ҫ���볬ʱ 
          {
            delay_ms(1);
            count_i++;
            if(count_i > 300)
            {
               CS1237_Data_OUT();
               CS1237_Data_HIGH();; // OUT��������
               CS1237_SCLK_HIGH(); // CLK��������
               return 1;//��ʱ����ֱ���˳�����
            }
          }
           
          for (i = 0; i < 26; i++) // 1-26th CLK   ǰ24��ADC ���� ��25 26���Ĵ���д����״̬         
          {
            CS1237_OneClk();
          }

          //CS1237_Data_HIGH(); // Force High  cs1237 direction input  27 �Ǹ������������ 
          CS1237_OneClk();    // 27th CLK  
                   
          
          CS1237_Data_OUT();
          CS1237_OneClk(); // 28th CLK   
          CS1237_OneClk(); // 29th CLK  �Ǹ����л������� 
          //CS1237_Data_OUT();                             
          for (i = 0; i < 7; i++) // 30~36th CLK  ��ʼ��λ������
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
   
          CS1237_Data_INPUT();     // cs1237 change to output direction�л��Ǹ��ߵķ���
         // CS1237_Data_LOW();          
          CS1237_OneClk();         // 37th CLK
       
          for (i = 0; i < 8; i++)  // 38~45th CLK  ����Ĵ����������ݻ�����Ĵ�����������  ��λ��
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




//********************************************ADоƬ��ʼ��****************************************************

    void C1237_Init(unsigned char refOutable, C1237_SPEED msel, C1237_PGA mselPga, C1237_CHN mselChn)
      {
        volatile u8 config = 0;                  
        config= ((refOutable&0x01)<<6)|((msel&0x03)<<4)|((mselPga&0x03)<<2)|((mselChn&0x03)<<0);
        C1237_WriteReg(config);
        ADC_REG_WRITE = config; // оƬ��������Ϣ 0C ��ʼ��
     
      }


//*******************************��ȡ24λADֵ����ȥ��Чλ***********************************
  double C1237_ReadResult()
    {
        u8 i=0;
        u8 j=0;
        unsigned char count_i=0;//�����ʱ��
        int32_t temp=0;
        uint32_t rdata=0; //��ȡ�������� 
        
         CS1237_Data_OUT();
         CS1237_Data_HIGH();
         CS1237_Data_INPUT();
         CS1237_SCLK_LOW();
         
         while (CS1237_Data_PIN_STATE())//��Ҫʵ�ʹ���ʹ����Ҫ���볬ʱ 
          {
            delay_ms(1);
            count_i++;
            if(count_i > 300)
            {
               CS1237_Data_OUT();
               CS1237_Data_HIGH();; // OUT��������
               CS1237_SCLK_HIGH(); // CLK��������
               return 1;//��ʱ����ֱ���˳�����
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
        
        
	//���һ������ת�������ڶ�ADC�������λ����ϵĶ�д����������Ҫ����46��CLK��������27��CLK
	//(��������24��CLK��ȡ���ݣ�Ȼ���ٷ���22��CLK)        
       //  for (j = 0; j < 22; j++)
       //   {

       //     CS1237_OneClk();

       //   }
        //CS1237_Data_OUT();
        //CS1237_Data_HIGH();     
        
       j = 24 - ADC_Bit;//i��ʾ��Ҫ������λ��
       temp >>= j;     //���������λ��
        
	if(rdata&0x00800000)// �ж��Ǹ��� ���λ24λ�Ƿ���λ
	{
          temp=-(((~rdata)&0x007FFFFF) + 1);// �����Դ��
	}
	else
	  temp=rdata; // �����Ĳ������Դ��



        return temp;

    }

 
//********************************************��ȡ�˲����ʵ��ֵ************************************************

//���㹫ʽ��Y=a*x+(1-a)*Y_last

//Y�Ǳ��������x�Ǳ������룬Y_last����һ�����
//a���˲�ϵ����1-aԽ�󣬹���(�ͺ�)Խ��˵�����ε��������һ�ε������ϵԽ��
//����һ���˲� �˲�ϵ��A��С��1����һ����ֵB��������ֵC out = b*A + C*(1-A)
//��������������ȡ������ADC����
long C1237_Read20BitAD(void)
  {
    long out,c;
    
    out = AD_Res_Last;
    c = C1237_ReadResult();
    if(c!=0)     // ������ȷ����
    {
      out = out*Lv_Bo + c*(1-Lv_Bo);
      AD_Res_Last = out;//����εļ������ŵ�ȫ�ֱ������汣��
    }
    return AD_Res_Last;
  }

/************************************************************ 
 * ����:        Vol_convert_Elec(float vol)
 * ˵��:        ��ͨ����ѹת��Ϊ4-20ma���� С�����  
************************************************************/
		
  double Vol_convert_Elec(double vol)
    {
      double elec;
      elec=(((vol-1)*16)/4)+4;
      return(elec);
    }

  
/***********************************************************************
�������ܣ���С��ת����IEEE754�������ͣ�����modbusЭ�鴫������
����������value С��ֵ    Holdaddr �ӻ����ּĴ�����ַ
***********************************************************************/		
 void Float_To_U32(float value, uint8_t *buf)
    {
        un_DtformConver Data;
        u16 Reg[2];//���ּĴ��� 
        u16 us_temp = 0;
        Data.ul_Temp = value;    //��ȡС��ֵ

       //Э�鶨���λ��ǰ
        Reg[0] = Data.uc_Buf[0];//��ȡ16~23λ 
        us_temp = Data.uc_Buf[1];        //��ȡ24~31λ
        Reg[0] |= us_temp <<8;  //��16λ���浽���ּĴ�����
        
        buf[1] = Reg[0] >> 8;
	buf[0] = Reg[0];


        Reg[1] = Data.uc_Buf[2];  //��ȡ���8λ
        us_temp = Data.uc_Buf[3];        //��ȡ8~15λ
        Reg[1] |= us_temp <<8;    //��16λ���浽���ּĴ�����  
        
        buf[3] = Reg[1] >> 8;
	buf[2] = Reg[1];

    }




  
  
  
  
 