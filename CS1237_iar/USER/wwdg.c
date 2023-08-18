#include "wwdg.h"
#include "stdio.h"


#define CounterInit 0x7f
#define window      0x77
//step=12288/16Mhz=0.768ms
//窗口看门狗到达CPU复位的最大时间:就是自减计数器从0x7F减到0x3F
//= (0x7F-0x3F) * step
//= 64 * step
//= 64 * 0.768ms
//= 49.152ms
//因此,窗口看门狗在0~49.152ms时间内没有喂狗,就会触发CPU复位

//不允许喂狗的时间:
//(0x7F-window) * step
//= (0x7F-0x77) * step
//= 7 * step 
//= 7 * 0.768ms 
//= 5.376ms
//不允许喂狗的时间为0.0ms ~ 5.376ms

//若使用软件喂狗,则允许喂狗的时间为5.376ms ~ (49.152-0.768*3)=46.848ms

//函数功能:打印CPU系统时钟源,及复位原因
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

  t=RST_GetFlagStatus(RST_FLAG_EMCF);//读取"EMC reset flag"
  if(t)
  {
    tmpReset_Record=(u8)(tmpReset_Record|RST_FLAG_EMCF);//记录"EMC reset flag"
    RST_ClearFlag(RST_FLAG_EMCF);
  }

  t=RST_GetFlagStatus(RST_FLAG_SWIMF);//读取"SWIM reset flag"
  if(t)
  {
    tmpReset_Record=(u8)(tmpReset_Record|RST_FLAG_SWIMF);//记录"SWIM reset flag"
    RST_ClearFlag(RST_FLAG_SWIMF);
  }

  t=RST_GetFlagStatus(RST_FLAG_ILLOPF);//读取"Illigal opcode reset flag"
  if(t)
  {
    tmpReset_Record=(u8)(tmpReset_Record|RST_FLAG_ILLOPF);//记录"Illigal opcode reset flag"
    RST_ClearFlag(RST_FLAG_ILLOPF);
  }

  t=RST_GetFlagStatus(RST_FLAG_IWDGF);//读取"Independent watchdog reset flag"
  if(t)
  {
    tmpReset_Record=(u8)(tmpReset_Record|RST_FLAG_IWDGF);//记录"Independent watchdog reset flag"
    RST_ClearFlag(RST_FLAG_IWDGF);
  }

  t=RST_GetFlagStatus(RST_FLAG_WWDGF);//读取"Window watchdog reset flag"
  if(t)
  {
    tmpReset_Record=(u8)(tmpReset_Record|RST_FLAG_WWDGF);//记录"Window watchdog reset flag"
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

//当WWDG_HW选项字节为1时,表示选择了硬件看门狗,看门狗将始终开启,而WDGA位将不起作用;
//窗口看门狗每过12288个CPU周期T递减一次
//看门狗计数器单步所需时长计算公式为:  step = 12288 * T
//WWDG_CR是7位的递减计数器T[6:0],由0x7F递减到0x3F会引起CPU复位
//窗口看门狗到达复位的最大时间为step * (0x7F-0x3F)

//WDGWR窗口寄存器是用来设置限位值;
//当WWDG_CR>WDGWR>0x3F时,不允许喂狗,否则会引起CPU复位
//当WDGWR>WWDG_CR>0x3F时,允许喂狗,不会引起CPU复位
//当WWDG_CR=0x3F时,会引起CPU复位

//例如,当CPU频率 f = 16Mhz时,step = 12288/16000000 = 0.000768s = 0.768ms,即一步为0.768ms
//窗口看门狗的超时时间最大为:0.000768s * 64 = 0.049152s = 49.152ms
//禁止喂狗时间 = (127- WDGWR) * 12288/16000000,这期间不允许喂狗,否则会引起CPU复位
//允许喂狗时间 = WDGWR * 12288/16000000,这期间允许喂狗,不会引起CPU复位

//函数功能:配置使用软件喂狗时间为5.376ms ~ 46.848ms
//窗口看门狗一旦启动,则无法关闭,除非发生复位,系统会自动关闭
void WWDG_Configuration(void) 
{
  WWDG_Init(CounterInit,window);
//设置WDGCR寄存器中的WDGA位将开启看门狗,在CPU复位之前,将不能关闭看门狗。
}

//函数功能:刷新窗口看门狗计数器,喂狗
void Refresh_WWDG_Window(void)//喂狗
{
  u8 CounterValue;

  CounterValue = (u8)(WWDG_GetCounter() & 0x7F);//读取看门狗计数器的值
  if(CounterValue < window)//如果"看门狗计数器的值" < "看门狗窗口的限位值"
  {
    WWDG_SetCounter(CounterInit);//喂狗,设置看门狗计数器
  }
}
