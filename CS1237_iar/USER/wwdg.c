#include "wwdg.h"
#include "stdio.h"


#define CounterInit 0x7f
#define window      0x77
//step=12288/16Mhz=0.768ms
//���ڿ��Ź�����CPU��λ�����ʱ��:�����Լ���������0x7F����0x3F
//= (0x7F-0x3F) * step
//= 64 * step
//= 64 * 0.768ms
//= 49.152ms
//���,���ڿ��Ź���0~49.152msʱ����û��ι��,�ͻᴥ��CPU��λ

//������ι����ʱ��:
//(0x7F-window) * step
//= (0x7F-0x77) * step
//= 7 * step 
//= 7 * 0.768ms 
//= 5.376ms
//������ι����ʱ��Ϊ0.0ms ~ 5.376ms

//��ʹ�����ι��,������ι����ʱ��Ϊ5.376ms ~ (49.152-0.768*3)=46.848ms

//��������:��ӡCPUϵͳʱ��Դ,����λԭ��
void SysRstSrcRecord(void)
{
  bool t;
  CLK_Source_TypeDef tmpCLK_Source_Type;
  u8 tmpReset_Record;

  tmpCLK_Source_Type=CLK_GetSYSCLKSource();
  if(tmpCLK_Source_Type==CLK_SOURCE_HSI)printf("\r\nClock Source is HSI!");
  if(tmpCLK_Source_Type==CLK_SOURCE_HSE)printf("\r\nClock Source is HSE!");
  if(tmpCLK_Source_Type==CLK_SOURCE_LSI)printf("\r\nClock Source is LSI!");
 
  tmpReset_Record=0;

  t=RST_GetFlagStatus(RST_FLAG_EMCF);//��ȡ"EMC reset flag"
  if(t)
  {
    tmpReset_Record=(u8)(tmpReset_Record|RST_FLAG_EMCF);//��¼"EMC reset flag"
    RST_ClearFlag(RST_FLAG_EMCF);
  }

  t=RST_GetFlagStatus(RST_FLAG_SWIMF);//��ȡ"SWIM reset flag"
  if(t)
  {
    tmpReset_Record=(u8)(tmpReset_Record|RST_FLAG_SWIMF);//��¼"SWIM reset flag"
    RST_ClearFlag(RST_FLAG_SWIMF);
  }

  t=RST_GetFlagStatus(RST_FLAG_ILLOPF);//��ȡ"Illigal opcode reset flag"
  if(t)
  {
    tmpReset_Record=(u8)(tmpReset_Record|RST_FLAG_ILLOPF);//��¼"Illigal opcode reset flag"
    RST_ClearFlag(RST_FLAG_ILLOPF);
  }

  t=RST_GetFlagStatus(RST_FLAG_IWDGF);//��ȡ"Independent watchdog reset flag"
  if(t)
  {
    tmpReset_Record=(u8)(tmpReset_Record|RST_FLAG_IWDGF);//��¼"Independent watchdog reset flag"
    RST_ClearFlag(RST_FLAG_IWDGF);
  }

  t=RST_GetFlagStatus(RST_FLAG_WWDGF);//��ȡ"Window watchdog reset flag"
  if(t)
  {
    tmpReset_Record=(u8)(tmpReset_Record|RST_FLAG_WWDGF);//��¼"Window watchdog reset flag"
    RST_ClearFlag(RST_FLAG_WWDGF);
  }

  if( (u8)(tmpReset_Record&RST_FLAG_EMCF) )
  {
    //RST_ClearFlag(RST_FLAG_EMCF);
    printf("\r\nEMC reset!");
  }
  if( (u8)(tmpReset_Record&RST_FLAG_SWIMF) )
  {
    //RST_ClearFlag(RST_FLAG_SWIMF);
    printf("\r\nSWIM reset!");
  }
  if( (u8)(tmpReset_Record&RST_FLAG_ILLOPF) )
  {
    //RST_ClearFlag(RST_FLAG_ILLOPF);
    printf("\r\nIlligal opcode reset!");
  }
  if( (u8)(tmpReset_Record&RST_FLAG_IWDGF) )
  {
    //RST_ClearFlag(RST_FLAG_IWDGF);
    printf("\r\nIWDG reset!");
  }
  if( (u8)(tmpReset_Record&RST_FLAG_WWDGF) )
  {
    //RST_ClearFlag(RST_FLAG_WWDGF);
    printf("\r\nWWDG reset!");
  }

  if(tmpReset_Record){ printf("\r\nCPU reset\r\n");}
}

//��WWDG_HWѡ���ֽ�Ϊ1ʱ,��ʾѡ����Ӳ�����Ź�,���Ź���ʼ�տ���,��WDGAλ����������;
//���ڿ��Ź�ÿ��12288��CPU����T�ݼ�һ��
//���Ź���������������ʱ�����㹫ʽΪ:  step = 12288 * T
//WWDG_CR��7λ�ĵݼ�������T[6:0],��0x7F�ݼ���0x3F������CPU��λ
//���ڿ��Ź����︴λ�����ʱ��Ϊstep * (0x7F-0x3F)

//WDGWR���ڼĴ���������������λֵ;
//��WWDG_CR>WDGWR>0x3Fʱ,������ι��,���������CPU��λ
//��WDGWR>WWDG_CR>0x3Fʱ,����ι��,��������CPU��λ
//��WWDG_CR=0x3Fʱ,������CPU��λ

//����,��CPUƵ�� f = 16Mhzʱ,step = 12288/16000000 = 0.000768s = 0.768ms,��һ��Ϊ0.768ms
//���ڿ��Ź��ĳ�ʱʱ�����Ϊ:0.000768s * 64 = 0.049152s = 49.152ms
//��ֹι��ʱ�� = (127- WDGWR) * 12288/16000000,���ڼ䲻����ι��,���������CPU��λ
//����ι��ʱ�� = WDGWR * 12288/16000000,���ڼ�����ι��,��������CPU��λ

//��������:����ʹ�����ι��ʱ��Ϊ5.376ms ~ 46.848ms
//���ڿ��Ź�һ������,���޷��ر�,���Ƿ�����λ,ϵͳ���Զ��ر�
void WWDG_Configuration(void) 
{
  WWDG_Init(CounterInit,window);
//����WDGCR�Ĵ����е�WDGAλ���������Ź�,��CPU��λ֮ǰ,�����ܹرտ��Ź���
}

//��������:ˢ�´��ڿ��Ź�������,ι��
void Refresh_WWDG_Window(void)//ι��
{
  u8 CounterValue;

  CounterValue = (u8)(WWDG_GetCounter() & 0x7F);//��ȡ���Ź���������ֵ
  if(CounterValue < window)//���"���Ź���������ֵ" < "���Ź����ڵ���λֵ"
  {
    WWDG_SetCounter(CounterInit);//ι��,���ÿ��Ź�������
  }
}
