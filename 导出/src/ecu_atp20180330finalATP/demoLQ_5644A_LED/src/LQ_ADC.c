/* /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/   
【平    台】北京龙邱智能科技MPC多功能开发板
【编    写】Chiu Sir
【E-mail  】chiusir@163.com
【软件版本】V1.0
【最后更新】2016年12月04日
【相关信息参考下列地址】
【网    站】http://www.lqist.cn
【淘宝店铺】http://shop36265907.taobao.com
------------------------------------------------
【dev.env.】Code Warrior 2.10
【Target  】MPC5644A 0M14X
【内部晶振】 khz
【外部晶振】40MHz-------
【总线频率】120MHz
/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/*/
#include "LQ_ADC.h"

/*******************************************************************************
Function Name : EQADC_Init
Engineer      : PetrS
Date          : Apr-20-2015
Parameters    : None
Modifies      : None
Returns       : NONE
Notes         : eQADC initialization
Issues        : NONE
*******************************************************************************/
void EQADC_Init(void)
{
    /* CBuffer0 */
    EQADC.CFPR[0].R = 0x08800401;/* Send CFIFO 0 a ADC0 configuration command */
                                 /* enable ADC0 & sets prescaler= divide by 10 */
    /* Enable also the ADC1 for better accuracy - see device errata document */
    /* CBuffer1 BN=1 */                               
    EQADC.CFPR[0].R = 0x8A800401;/* Send CFIFO 0 a ADC1 configuration command */
                                 /* enable ADC1 & sets prescaler= divide by 10 */
                                 /* EOQ */
                                      
                                      
    EQADC.CFCR[0].R = 0x0410;    /* Trigger CFIFO 0 using Single Scan SW mode */
    while (EQADC.FISR[0].B.EOQF !=1) {} /* Wait for End Of Queue flag */
    EQADC.FISR[0].R = 0x10000000;  
}

/*******************************************************************************
Function Name : ConvertChannel
Engineer      : PetrS
Date          : Apr-20-2015
Parameters    : channel .. channel number to sample
				bCal .. 1 sample with calibration
						0 sample without calibartion
Modifies      : None
Returns       : sampled value
Notes         : 
Issues        : NONE
*******************************************************************************/
uint32_t ConvertChannel(int32_t channel, int32_t bCal)
{
    uint32_t ret = 0;

    /* Conversion command: convert "channel" */
    /* with ADC0, set EOQ, and send result to RFIFO 0 */ 
    /* 128 adc clocks sampling */
    EQADC.CFPR[0].R = 0x800C0000 | (channel<<8) | (bCal<<24); 
    EQADC.CFCR[0].R = 0x0410;   /* Trigger CFIFO 0 using Single Scan SW mode */
    /* Wait for RFIFO 0's Drain Flag to set */
    while (EQADC.FISR[0].B.RFDF != 1){}
    
    ret = EQADC.RFPR[0].R;
    
    /* Clear RFIFO 0's Drain Flag */
    /* Clear CFIFO's End of Queue flag */
    /* RFDF + EOQF */
    EQADC.FISR[0].R = 0x10020000;
    
    return ret;
}

/*******************************************************************************
Function Name : compare
Engineer      : PetrS
Date          : Apr-20-2015
Parameters    : 
Modifies      : 
Returns       :  
Notes         : compare two values
Issues        : NONE
*******************************************************************************/
int compare (const void * p0, const void * p1)
{
    uint32_t a0 = (uint32_t) *(uint32_t*)p0;
    uint32_t a1 = (uint32_t) *(uint32_t*)p1;

    if(a0==a1) return 0;
    if(a0<a1) return -1;
    else return 1;
}

/*******************************************************************************
Function Name : ConvertChannel_Median
Engineer      : PetrS
Date          : Apr-20-2015
Parameters    : None
Modifies      : None
Returns       : median 
Notes         : finds median from 21 results
Issues        : NONE
*******************************************************************************/
uint32_t ConvertChannel_Median(int32_t channel, int32_t bCal)
{
    uint32_t results[21];
    //uint32_t sorted_results[11];
            
    int32_t i = 0;
    for(i=0;i<sizeof(results)/sizeof(results[0]);i++)
    {
        results[i] = ConvertChannel(channel, bCal);
    }
    
    /* sort results */
    qsort((void*)results,11,sizeof(results[i]),compare);
    
    return results[10]; /* median */
}

/*******************************************************************************
Function Name : EQADC_PowerUpCalibration
Engineer      : PetrS
Date          : Apr-20-2015
Parameters    : None
Modifies      : None
Returns       : 0 if done properly
Notes         : performs eQADC calibartion
Issues        : NONE
*******************************************************************************/
int32_t EQADC_PowerUpCalibration(void) 
{
    unsigned long raw_res75, raw_res25;
    unsigned long cal_res75, cal_res25;
    
    float gccf, occf;
    
    unsigned short gain;
    short offset;
    
    /* Convert channel 43 (75%VRH)with CAL=0 */
    //raw_res75 = ConvertChannel(43,0);    
    raw_res75 = ConvertChannel_Median(43,0);
    /* Convert channel 44 (25%VRH)with CAL=0, 64 sample time (errata) */
    //raw_res25 = ConvertChannel(44,0);
    raw_res25 = ConvertChannel_Median(44,0);
   
    /* Compute Gain and Offset */
    gccf = (float)(8192.0)/(raw_res75 - raw_res25);
    gain = gccf * 16384;
    occf = (float)(IDEAL_RES75 - (gccf * raw_res75) - 2.0);
    offset = occf;

    /* now store Gain in ADCn_GCCR */
    EQADC.CFPR[0].R = (gain<<8) | 0x00000004;
    /* now store offset in ADCn_OCCR */
    EQADC.CFPR[0].R = ((unsigned short)offset << 8) | 0x00000005;

    /* now store Gain in ADCn_AGR */
    EQADC.CFPR[0].R = (gain<<8) | 0x00000031;
    /* now store offset in ADCn_ACR */
    EQADC.CFPR[0].R = ((unsigned short)offset << 8) | 0x00000032 | 0x80000000;
    
    EQADC.CFCR[0].R = 0x0410;    /* Trigger CFIFO 0 using Single Scan SW mode */
    while (EQADC.FISR[0].B.EOQF !=1) {} /* Wait for End Of Queue flag */
    EQADC.FISR[0].R = 0x10000000;
    
    cal_res75 = ConvertChannel(43,1);
    cal_res25 = ConvertChannel(44,1);    
    
    // Check the results
    if ((abs(cal_res75 - IDEAL_RES75) > 0xF) || (abs(cal_res25 - IDEAL_RES25) > 0xF)) 
    {
	    /* error */
	    return 1;
    }
    
    return 0; /* no error */    
}



void initADC0(void) {
  EQADC.CFPR[0].R = 0x80801001;       /* Send CFIFO 0 a ADC0 configuration command */
                                      /* enable ADC0 & sets prescaler= divide by 2 */ 
  EQADC.CFCR[0].R = 0x0410;           /* Trigger CFIFO 0 using Single Scan SW mode */
  while (EQADC.FISR[0].B.EOQF !=1) {} /* Wait for End Of Queue flag */
  EQADC.FISR[0].B.EOQF = 1;           /* Clear End Of Queue flag */
}

void SendConvCmd (void) {
  EQADC.CFPR[0].R = 0x00000000; /* Conversion command: convert channel 0 */
                                /* with ADC0, set EOQ, and send result to RFIFO 0 */ 
  EQADC.CFCR[0].R = 0x0410;     /* Trigger CFIFO 0 using Single Scan SW mode */
}

uint32_t ReadResult(void) 
{ 
  uint32_t Result,ResultInMv;
   
  SendConvCmd();                /* Send one conversion command */
  while (EQADC.FISR[0].B.RFDF != 1){}      /* Wait for RFIFO 0's Drain Flag to set */
  Result = EQADC.RFPR[0].R;                /* ADC result */ 
  ResultInMv = (uint32_t)((5000*Result)/0x3FFC);  /* ADC result in millivolts */        
  EQADC.FISR[0].B.RFDF = 1;                /* Clear RFIFO 0's Drain Flag */
  EQADC.FISR[0].B.EOQF = 1;                /* Clear CFIFO's End of Queue flag */
  return Result;
}

