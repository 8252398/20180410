/*------------------------------------------------------------------------
   版权 2017-2027 中国航空工业集团第六三一所第十八研究室
   
   对本文件的拷贝、发布、修改或者其他任何用途必须得到
   中国航空工业集团第六三一所的书面协议许可
   
   File : eQADC.c
   
   Description : eQADC驱动
   
   
   Revision History:
   rev.0.01 2017-11-13,马振华,创建
------------------------------------------------------------------------*/
#include "mpc5644a.h"  
#include "typedefs.h"  
#include "eQADC.h"

#define CFIFO_PUSH 0xFFF80010   /*PUSH 32位宽*/
#define RFIFO_POP 0xFFF80032    /*POP 取寄存器中低十六位的值*/   

vuint32_t RQUEUE[40] = {0};
vuint32_t CQUEUE0[40] = {0};


/************************************************************************
* @brief 
* 		eQadcChConfig	配置模拟量输入管脚
* @param[in]  port:端口
*			  hys:Input hysteresis
*			  wpe:Weak pullup/down enable
*			  wps:Weak pullup/down select
* @param[out] 无		  
* @return     无
************************************************************************/
void eQadcChConfig(uint16_t port, uint16_t hys, uint16_t wpe, uint16_t wps)
{ 
	SIU.PCR[port].B.PA = 1;  /* ANx */  
	SIU.PCR[port].B.IBE = 1; /* INPUT */
	SIU.PCR[port].B.OBE = 0; /* OUTPUT */
	SIU.PCR[port].B.DSC = 0; /* 00 10 pF drive strength */
	SIU.PCR[port].B.ODE = 0;
	SIU.PCR[port].B.HYS = hys; /* 0 Disable hysteresis for the pad,1 Enable hysteresis for the pad */
	SIU.PCR[port].B.SRC = 0; 
	SIU.PCR[port].B.WPE = wpe; /* 0 Disable weak pull device for the pad,1 Enable weak pull device for the pad. */
	SIU.PCR[port].B.WPS = wps; /* 0 Pulldown is enabled for the pad,1 Pullup is enabled for the pad. */ 	
}

/************************************************************************
* @brief 
* 		eQadcPcrConfig	配置模拟量输入管脚AN12~AN15
* @param[in]  无
* @param[out] 无		  
* @return     无
************************************************************************/
void eQadcPcrConfig(void)
{
	uint16_t hys = 0, wpe = 0, wps = 0;
	
	/* 喷油输出管脚配置 */
	eQadcChConfig(215, hys, wpe, wps); /* SCR_OUT温度 AN12 */
	eQadcChConfig(216, hys, wpe, wps); /* UREA_TANK_TEMP */
	eQadcChConfig(217, hys, wpe, wps); /* 空 */
	eQadcChConfig(218, hys, wpe, wps); /* 24V监控 */
}
 
/************************************************************************
* @brief 
* 		EDMAInitFcn	EDMA配置
* @param[in]  无
* @param[out] 无		  
* @return     无
************************************************************************/
void EDMAInitFcn(void) 
{	
	/* edma_global_config */
    EDMA.CR.R = 0x0000E400; /* Use fixed priority arbitration for DMA groups and channels */    
    EDMA.ERQRH.R = 0x00000000;
    EDMA.ERQRL.R = 0x00000003;  /* 使�0~1通道 */ 
    EDMA.EEIRH.R = 0x00000000;  /* 禁止错误中断 */
    EDMA.EEIRL.R = 0x00000000;
    
    /* 配置CFIFO 00 对应的TCD edma_eqadc0_config */
    EDMA.TCD[0].SADDR = (vuint32_t)(&CQUEUE0[0]); /* 源地址 */
	EDMA.TCD[0].DADDR = (vuint32_t)(&EQADC.CFPR[0].R); /*目标地址(vuint32_t)&EQADC.CFPR[0].R*/
	EDMA.TCD[0].SMOD = 0x0; /* 源地址模数，表示最低的n位可以改变，从而提供一个2^n字节的环形队列 */
	EDMA.TCD[0].DMOD = 0x0; /* 目标地址模数 */   
	EDMA.TCD[0].SSIZE = 0x2; /* 目标传输大小32位，即每次从源地址读32位数 */
	EDMA.TCD[0].DSIZE = 0x2; /* 源传输大小，0-1B,1-2B,2-4B,3-8B,5-32B */   
	EDMA.TCD[0].SOFF = 0x0; /*有符号源偏移量*/
	EDMA.TCD[0].NBYTES = 0x4; /*一次内层次要循环传输的字节数,即每次DMA传输的字节数*/
	EDMA.TCD[0].SLAST = 0; /* -160 0xFFFFFF60末级目标地址调整值，主循环结束时加到源地址上*/   
	EDMA.TCD[0].DOFF = 0x0; /*有符号目标地址偏移量*/
	EDMA.TCD[0].DLAST_SGA = 0; /*末级有符号目标地址偏移量*/
	EDMA.TCD[0].BITER = 1; /*主循环迭代计数初始值40*/
	EDMA.TCD[0].CITER = 1; /*主循环迭代计数当前值*/   
	EDMA.TCD[0].BWC = 0x0; /*带宽控制：0-无DMA传输暂停，2-为减少总线上其他设备访问相同资源的时间延迟，
	                         eDMA完成每次读/写访问后暂停4个时钟周期 ，*/
	EDMA.TCD[0].MAJORLINKCH = 0x0; /*主链接通道号*/
	EDMA.TCD[0].MAJORE_LINK = 0x0; /*主循环完成时，通道链接禁止*/
	EDMA.TCD[0].DONE = 0x00; /* 通道服务完成，EDMA在主循环完成时设置该位 */
	EDMA.TCD[0].ACTIVE = 0x00;  /* 通道活跃，edma在每次次要循环开始时置位，结束时清零 */
	EDMA.TCD[0].E_SG = 0x0; /* 分散/聚合操作禁止 */
	EDMA.TCD[0].D_REQ = 0x0; /* 主循环完成时禁止通道请求。在使用硬件启动edma请求时，如果该控制位被设置，
	                            则在主循环结束时，会禁止DMA请求 */
	EDMA.TCD[0].INT_HALF = 0x0; /* 主循环执行一半时不中断内核 */
	EDMA.TCD[0].INT_MAJ = 0x1; /* 1-主循环完成中断内核 */
	EDMA.TCD[0].START = 0x0; /* 通道strat位，在想通过软件发起DMA服务请求时，可以通过软件向该位写1明确启动该edma通道。
	                            一旦DMA通道时因该位设置开始执行，该通道在完成所有数据传输前不能被停止 */   
	                            
	/* 配置RFIFO 00 对应的TCD */
    EDMA.TCD[1].SADDR = (vuint32_t)(&EQADC.RFPR[0].R); /* 源地址 */
	EDMA.TCD[1].DADDR = (vuint32_t)(&RQUEUE[0]); /*目标地址(vuint32_t)&EQADC.CFPR[0].R*/
	EDMA.TCD[1].SMOD = 0x0; /* 源地址模数，表示最低的n位可以改变，从而提供一个2^n字节的环形队列 */
	EDMA.TCD[1].DMOD = 0x0; /* 目标地址模数 */   
	EDMA.TCD[1].SSIZE = 0x2; /* 目标传输大小16位，即每次从源地址读16位数 */
	EDMA.TCD[1].DSIZE = 0x2; /* 源传输大小，0-1B,1-2B,2-4B,3-8B,5-32B */   
	EDMA.TCD[1].SOFF = 0x0; /*有符号源偏移量,每次源读取操作完成后加到SADDR上*/
	EDMA.TCD[1].NBYTES = 0x4; /*一次内层次要循环传输的字节数,即每次DMA传输的字节数*/
	EDMA.TCD[1].SLAST = 0; /*末级目标地址调整值，主循环结束时加到源地址上*/   
	EDMA.TCD[1].DOFF = 0x0; /*有符号目标地址偏移量*/
	EDMA.TCD[1].DLAST_SGA = 0; /*末级有符号目标地址偏移量-80*/
	EDMA.TCD[1].BITER = 1; /*主循环迭代计数初始值*/;
	EDMA.TCD[1].CITER = 1; /*主循环迭代计数当前值*/;    
	EDMA.TCD[1].BWC = 0x0; /*带宽控制：无DMA传输暂停*/
	EDMA.TCD[1].MAJORLINKCH = 0x0; /*主链接通道号*/
	EDMA.TCD[1].MAJORE_LINK = 0x0; /*主循环完成时，通道链接禁止*/
	EDMA.TCD[1].DONE = 0x00; /* 通道服务完成，EDMA在主循环完成时设置该位 */
	EDMA.TCD[1].ACTIVE = 0x00;  /* 通道活跃，edma在每次次要循环开始时置位，结束时清零 */
	EDMA.TCD[1].E_SG = 0x0; /* 分散/聚合操作禁止 */
	EDMA.TCD[1].D_REQ = 0x0; /* 主循环完成时禁止通道请求。在使用硬件启动edma请求时，如果该控制位被设置，
	                            则在主循环结束时，会禁止DMA请求 */
	EDMA.TCD[1].INT_HALF = 0x0; /* 主循环执行一半时不中断内核 */
	EDMA.TCD[1].INT_MAJ = 0x0; /* 主循环完成中断禁止 */
	EDMA.TCD[1].START = 0x0; /* 通道strat位，在想通过软件发起DMA服务请求时，可以通过软件向该位写1明确启动该edma通道。
	                            一旦DMA通道时因该位设置开始执行，该通道在完成所有数据传输前不能被停止 */          
}

/************************************************************************
* @brief 
* 		eQADCInitFcn	eQADC配置
* @param[in]  无
* @param[out] 无		  
* @return     无
************************************************************************/
void eQADCInitFcn(void) 
{
    /* 转换命令配置 */	
    CQUEUE0[0] = 0x80001000;  /*  16通道轨压采集使能时间戳 
                                  bit1：EOQ=1,单次扫描模式且处于命令队列末尾置1（1 Last entry of the CQueue） 
	                              bit8~11:MESSAGE_TAG[0:3]=0,Result is sent to RFIFO 0 (0~5对应RFIFO0~5)
	                              bit12~13:lst[0:1]=0,Sampling cycles(ADC Clock Cycles)=2 (1-8,2-64,3-128)
	                              bit14:TSR=1,通道使能时间戳(Return conversion time stamp after the conversion result.) */
	
 
    /* eqadc_global_config */
    EQADC.MCR.R = 0x00000000;		/* ICEA0=0 Disable immediate conversion command request
                                       ICEA1=0 Disable immediate conversion command request  
                                       ESSIE = 0 EQADC SSI is disabled
                                       DBG=0 Do not enter debug mode */
	EQADC.NMSFR.R = 0x00000000;	/* Null Message Format:The value written to this register will be sent as 
	                               a null message when serial transmissions from the EQADC SSI are enabled*/
	EQADC.ETDFR.R = 0x00000000;	/* DFL(bit28-31):Digital Filter Length = 2  */

	
	/* CFIFO0配置 */
	EQADC.CFCR[0].R = 0x0010; /* Trigger CFIFO 0 using Single Scan SW mode 
	                             CFEEE0= 0 CFIFO0 has a normal value of entries  
	                             STRME0=0 Streaming mode of CFIFO0 is disabled
	                             SSE0=0 No effect.
	                             CFINV0=0 No effect.
	                             MODE0=1 Software Trigger, Single Scan
	                             AMODE0=0 Disable CFIFO0 Advance Trigger Operation Mode 0 */ 
	EQADC.IDCR[0].R = 0x0000; /* Disable all interrupt and DMA request. */
	
	/* eqadc_adc0_config */
    EQADC.CFPR[0].B.CFPUSH = 0x00000802; /* 初始化时间戳控制寄存器0 等价于EQADC.CFPR[0].R，ADC_TSCR=0X0008,Clock Divide Factor for Time Stamp 16分频*/
    EQADC.CFPR[0].B.CFPUSH = 0x00000003; /* 初始化时基计数寄存器0 ADC_TBCR=0X0000,设置时基计数器为0*/
    EQADC.CFPR[0].B.CFPUSH = 0x00800301; /* 初始化ADC控制寄存器0 ADC0_CR=0X8003,bit12~15=0x03,Clock Divide Factor for Time Stamp 8分频  (0x9-20分频，即最大速度12MHz) 
                                                bit4=0 External multiplexer disabled; no external multiplexer channels can be selected.
                                                bit0=1 1 ADC is enabled and ready to perform A/D conversions. */
    EQADC.CFPR[0].B.CFPUSH = 0x82800301; /* 初始化ADC控制寄存器1 ADC1_CR=0X8003 */
    EQADC.CFCR[0].B.SSE = 0x01; /* Trigger the CFIFO0 to config ACDxs，Set the SSSx bit */
   
    
    while (EQADC.CFSR.B.CFS0 == 0x3) /* Wait for trigger state to end so we konw queue is done，Current CFIFO Status=3，CFIFO is triggered*/
    {
    	;/* no deal with */
    }
    
    EQADC.FISR[0].B.EOQF = 0x01; /* Clear End Of Queue flag */   
    EQADC.CFCR[0].R = 0x0000; /* disable the queue in preparation for queue mode change */ 
    
    while (EQADC.CFSR.B.CFS0 != 0) /* wait for queue to go to idle before setting mode to user setting */  
    {
    	;/* no deal with */
    }
    
    /* CFIFO0配置 */
    EQADC.IDCR[0].R = 0x0003; /* 0x0303 CFIFO满禁止DMA请求RFIFO空使能DMA请求, CFFE0 = 1 Enable CFIFO Fill DMA or Interrupt request
                                 CFFS0 = 1 Generate DMA request to move data from the system memory to CFIFOx  
                                         0 Generate interrupt request to move data from the system memory to CFIFOx
                                 RFDE0=1 Enable RFIFO Drain DMA or Interrupt request
                                 RFDS0=1 Generate DMA request to move data from RFIFOx to the system memory
                                       0 Generate interrupt request to move data from RFIFOx to the system memory */  
    EQADC.CFCR[0].R = 0x0010; /* Trigger CFIFO 0 using Single Scan SW mode， 0x0090为软件连续触发模式 */ 
}

/************************************************************************
* @brief 
* 		adConvCmdSend	发送AD转换命令
* @param[in]  ch:模拟量通道号
* @param[out] 无		  
* @return     无
************************************************************************/
void adConvCmdSend (uint8_t ch) 
{
    EQADC.CFPR[0].R = 0x00100000 + (ch << 8); /* Conversion command: convert channel ch 
                                                 with ADC0, set EOQ, and send result to RFIFO 1 */ 
    EQADC.CFCR[0].R = 0x0410; /* Trigger CFIFO 0 using Single Scan SW mode */
}

/************************************************************************
* @brief 
* 		adResultRead	读取AD转换电压值
* @param[in]  ch:模拟量通道号
* @param[out] 无		  
* @return     AD转换后电压值
************************************************************************/
uint32_t adResultRead(uint8_t ch) 
{ 
    uint32_t Result,ResultInMv;
   
    adConvCmdSend(ch); /* Send one conversion command */
    
    while (EQADC.FISR[1].B.RFDF != 1) /* Wait for RFIFO 1's Drain Flag to set */
    {
    	;/* no deal with */
    }
    
    Result = EQADC.RFPR[1].R;                /* ADC result */ 
    ResultInMv = (uint32_t)((5000*Result)/0x3FFC);  /* ADC result in millivolts */        
    EQADC.FISR[1].B.RFDF = 1;                /* Clear RFIFO 1's Drain Flag */
    EQADC.FISR[0].B.EOQF = 1;                /* Clear CFIFO's End of Queue flag */
   
    return ResultInMv;
}



