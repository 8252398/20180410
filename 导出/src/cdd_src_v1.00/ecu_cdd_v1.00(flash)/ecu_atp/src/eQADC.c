/*------------------------------------------------------------------------
   ��Ȩ 2017-2027 �й����չ�ҵ���ŵ�����һ����ʮ���о���
   
   �Ա��ļ��Ŀ������������޸Ļ��������κ���;����õ�
   �й����չ�ҵ���ŵ�����һ��������Э�����
   
   File : eQADC.c
   
   Description : eQADC����
   
   
   Revision History:
   rev.0.01 2017-11-13,����,����
------------------------------------------------------------------------*/
#include "mpc5644a.h"  
#include "typedefs.h"  
#include "eQADC.h"

#define CFIFO_PUSH 0xFFF80010   /*PUSH 32λ��*/
#define RFIFO_POP 0xFFF80032    /*POP ȡ�Ĵ����е�ʮ��λ��ֵ*/   

vuint32_t RQUEUE[40] = {0};
vuint32_t CQUEUE0[40] = {0};


/************************************************************************
* @brief 
* 		eQadcChConfig	����ģ��������ܽ�
* @param[in]  port:�˿�
*			  hys:Input hysteresis
*			  wpe:Weak pullup/down enable
*			  wps:Weak pullup/down select
* @param[out] ��		  
* @return     ��
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
* 		eQadcPcrConfig	����ģ��������ܽ�AN12~AN15
* @param[in]  ��
* @param[out] ��		  
* @return     ��
************************************************************************/
void eQadcPcrConfig(void)
{
	uint16_t hys = 0, wpe = 0, wps = 0;
	
	/* ��������ܽ����� */
	eQadcChConfig(215, hys, wpe, wps); /* SCR_OUT�¶� AN12 */
	eQadcChConfig(216, hys, wpe, wps); /* UREA_TANK_TEMP */
	eQadcChConfig(217, hys, wpe, wps); /* �� */
	eQadcChConfig(218, hys, wpe, wps); /* 24V��� */
}
 
/************************************************************************
* @brief 
* 		EDMAInitFcn	EDMA����
* @param[in]  ��
* @param[out] ��		  
* @return     ��
************************************************************************/
void EDMAInitFcn(void) 
{	
	/* edma_global_config */
    EDMA.CR.R = 0x0000E400; /* Use fixed priority arbitration for DMA groups and channels */    
    EDMA.ERQRH.R = 0x00000000;
    EDMA.ERQRL.R = 0x00000003;  /* ʹ�0~1ͨ�� */ 
    EDMA.EEIRH.R = 0x00000000;  /* ��ֹ�����ж� */
    EDMA.EEIRL.R = 0x00000000;
    
    /* ����CFIFO 00 ��Ӧ��TCD edma_eqadc0_config */
    EDMA.TCD[0].SADDR = (vuint32_t)(&CQUEUE0[0]); /* Դ��ַ */
	EDMA.TCD[0].DADDR = (vuint32_t)(&EQADC.CFPR[0].R); /*Ŀ���ַ(vuint32_t)&EQADC.CFPR[0].R*/
	EDMA.TCD[0].SMOD = 0x0; /* Դ��ַģ������ʾ��͵�nλ���Ըı䣬�Ӷ��ṩһ��2^n�ֽڵĻ��ζ��� */
	EDMA.TCD[0].DMOD = 0x0; /* Ŀ���ַģ�� */   
	EDMA.TCD[0].SSIZE = 0x2; /* Ŀ�괫���С32λ����ÿ�δ�Դ��ַ��32λ�� */
	EDMA.TCD[0].DSIZE = 0x2; /* Դ�����С��0-1B,1-2B,2-4B,3-8B,5-32B */   
	EDMA.TCD[0].SOFF = 0x0; /*�з���Դƫ����*/
	EDMA.TCD[0].NBYTES = 0x4; /*һ���ڲ��Ҫѭ��������ֽ���,��ÿ��DMA������ֽ���*/
	EDMA.TCD[0].SLAST = 0; /* -160 0xFFFFFF60ĩ��Ŀ���ַ����ֵ����ѭ������ʱ�ӵ�Դ��ַ��*/   
	EDMA.TCD[0].DOFF = 0x0; /*�з���Ŀ���ַƫ����*/
	EDMA.TCD[0].DLAST_SGA = 0; /*ĩ���з���Ŀ���ַƫ����*/
	EDMA.TCD[0].BITER = 1; /*��ѭ������������ʼֵ40*/
	EDMA.TCD[0].CITER = 1; /*��ѭ������������ǰֵ*/   
	EDMA.TCD[0].BWC = 0x0; /*������ƣ�0-��DMA������ͣ��2-Ϊ���������������豸������ͬ��Դ��ʱ���ӳ٣�
	                         eDMA���ÿ�ζ�/д���ʺ���ͣ4��ʱ������ ��*/
	EDMA.TCD[0].MAJORLINKCH = 0x0; /*������ͨ����*/
	EDMA.TCD[0].MAJORE_LINK = 0x0; /*��ѭ�����ʱ��ͨ�����ӽ�ֹ*/
	EDMA.TCD[0].DONE = 0x00; /* ͨ��������ɣ�EDMA����ѭ�����ʱ���ø�λ */
	EDMA.TCD[0].ACTIVE = 0x00;  /* ͨ����Ծ��edma��ÿ�δ�Ҫѭ����ʼʱ��λ������ʱ���� */
	EDMA.TCD[0].E_SG = 0x0; /* ��ɢ/�ۺϲ�����ֹ */
	EDMA.TCD[0].D_REQ = 0x0; /* ��ѭ�����ʱ��ֹͨ��������ʹ��Ӳ������edma����ʱ������ÿ���λ�����ã�
	                            ������ѭ������ʱ�����ֹDMA���� */
	EDMA.TCD[0].INT_HALF = 0x0; /* ��ѭ��ִ��һ��ʱ���ж��ں� */
	EDMA.TCD[0].INT_MAJ = 0x1; /* 1-��ѭ������ж��ں� */
	EDMA.TCD[0].START = 0x0; /* ͨ��stratλ������ͨ���������DMA��������ʱ������ͨ��������λд1��ȷ������edmaͨ����
	                            һ��DMAͨ��ʱ���λ���ÿ�ʼִ�У���ͨ��������������ݴ���ǰ���ܱ�ֹͣ */   
	                            
	/* ����RFIFO 00 ��Ӧ��TCD */
    EDMA.TCD[1].SADDR = (vuint32_t)(&EQADC.RFPR[0].R); /* Դ��ַ */
	EDMA.TCD[1].DADDR = (vuint32_t)(&RQUEUE[0]); /*Ŀ���ַ(vuint32_t)&EQADC.CFPR[0].R*/
	EDMA.TCD[1].SMOD = 0x0; /* Դ��ַģ������ʾ��͵�nλ���Ըı䣬�Ӷ��ṩһ��2^n�ֽڵĻ��ζ��� */
	EDMA.TCD[1].DMOD = 0x0; /* Ŀ���ַģ�� */   
	EDMA.TCD[1].SSIZE = 0x2; /* Ŀ�괫���С16λ����ÿ�δ�Դ��ַ��16λ�� */
	EDMA.TCD[1].DSIZE = 0x2; /* Դ�����С��0-1B,1-2B,2-4B,3-8B,5-32B */   
	EDMA.TCD[1].SOFF = 0x0; /*�з���Դƫ����,ÿ��Դ��ȡ������ɺ�ӵ�SADDR��*/
	EDMA.TCD[1].NBYTES = 0x4; /*һ���ڲ��Ҫѭ��������ֽ���,��ÿ��DMA������ֽ���*/
	EDMA.TCD[1].SLAST = 0; /*ĩ��Ŀ���ַ����ֵ����ѭ������ʱ�ӵ�Դ��ַ��*/   
	EDMA.TCD[1].DOFF = 0x0; /*�з���Ŀ���ַƫ����*/
	EDMA.TCD[1].DLAST_SGA = 0; /*ĩ���з���Ŀ���ַƫ����-80*/
	EDMA.TCD[1].BITER = 1; /*��ѭ������������ʼֵ*/;
	EDMA.TCD[1].CITER = 1; /*��ѭ������������ǰֵ*/;    
	EDMA.TCD[1].BWC = 0x0; /*������ƣ���DMA������ͣ*/
	EDMA.TCD[1].MAJORLINKCH = 0x0; /*������ͨ����*/
	EDMA.TCD[1].MAJORE_LINK = 0x0; /*��ѭ�����ʱ��ͨ�����ӽ�ֹ*/
	EDMA.TCD[1].DONE = 0x00; /* ͨ��������ɣ�EDMA����ѭ�����ʱ���ø�λ */
	EDMA.TCD[1].ACTIVE = 0x00;  /* ͨ����Ծ��edma��ÿ�δ�Ҫѭ����ʼʱ��λ������ʱ���� */
	EDMA.TCD[1].E_SG = 0x0; /* ��ɢ/�ۺϲ�����ֹ */
	EDMA.TCD[1].D_REQ = 0x0; /* ��ѭ�����ʱ��ֹͨ��������ʹ��Ӳ������edma����ʱ������ÿ���λ�����ã�
	                            ������ѭ������ʱ�����ֹDMA���� */
	EDMA.TCD[1].INT_HALF = 0x0; /* ��ѭ��ִ��һ��ʱ���ж��ں� */
	EDMA.TCD[1].INT_MAJ = 0x0; /* ��ѭ������жϽ�ֹ */
	EDMA.TCD[1].START = 0x0; /* ͨ��stratλ������ͨ���������DMA��������ʱ������ͨ��������λд1��ȷ������edmaͨ����
	                            һ��DMAͨ��ʱ���λ���ÿ�ʼִ�У���ͨ��������������ݴ���ǰ���ܱ�ֹͣ */          
}

/************************************************************************
* @brief 
* 		eQADCInitFcn	eQADC����
* @param[in]  ��
* @param[out] ��		  
* @return     ��
************************************************************************/
void eQADCInitFcn(void) 
{
    /* ת���������� */	
    CQUEUE0[0] = 0x80001000;  /*  16ͨ����ѹ�ɼ�ʹ��ʱ��� 
                                  bit1��EOQ=1,����ɨ��ģʽ�Ҵ����������ĩβ��1��1 Last entry of the CQueue�� 
	                              bit8~11:MESSAGE_TAG[0:3]=0,Result is sent to RFIFO 0 (0~5��ӦRFIFO0~5)
	                              bit12~13:lst[0:1]=0,Sampling cycles(ADC Clock Cycles)=2 (1-8,2-64,3-128)
	                              bit14:TSR=1,ͨ��ʹ��ʱ���(Return conversion time stamp after the conversion result.) */
	
 
    /* eqadc_global_config */
    EQADC.MCR.R = 0x00000000;		/* ICEA0=0 Disable immediate conversion command request
                                       ICEA1=0 Disable immediate conversion command request  
                                       ESSIE = 0 EQADC SSI is disabled
                                       DBG=0 Do not enter debug mode */
	EQADC.NMSFR.R = 0x00000000;	/* Null Message Format:The value written to this register will be sent as 
	                               a null message when serial transmissions from the EQADC SSI are enabled*/
	EQADC.ETDFR.R = 0x00000000;	/* DFL(bit28-31):Digital Filter Length = 2  */

	
	/* CFIFO0���� */
	EQADC.CFCR[0].R = 0x0010; /* Trigger CFIFO 0 using Single Scan SW mode 
	                             CFEEE0= 0 CFIFO0 has a normal value of entries  
	                             STRME0=0 Streaming mode of CFIFO0 is disabled
	                             SSE0=0 No effect.
	                             CFINV0=0 No effect.
	                             MODE0=1 Software Trigger, Single Scan
	                             AMODE0=0 Disable CFIFO0 Advance Trigger Operation Mode 0 */ 
	EQADC.IDCR[0].R = 0x0000; /* Disable all interrupt and DMA request. */
	
	/* eqadc_adc0_config */
    EQADC.CFPR[0].B.CFPUSH = 0x00000802; /* ��ʼ��ʱ������ƼĴ���0 �ȼ���EQADC.CFPR[0].R��ADC_TSCR=0X0008,Clock Divide Factor for Time Stamp 16��Ƶ*/
    EQADC.CFPR[0].B.CFPUSH = 0x00000003; /* ��ʼ��ʱ�������Ĵ���0 ADC_TBCR=0X0000,����ʱ��������Ϊ0*/
    EQADC.CFPR[0].B.CFPUSH = 0x00800301; /* ��ʼ��ADC���ƼĴ���0 ADC0_CR=0X8003,bit12~15=0x03,Clock Divide Factor for Time Stamp 8��Ƶ  (0x9-20��Ƶ��������ٶ�12MHz) 
                                                bit4=0 External multiplexer disabled; no external multiplexer channels can be selected.
                                                bit0=1 1 ADC is enabled and ready to perform A/D conversions. */
    EQADC.CFPR[0].B.CFPUSH = 0x82800301; /* ��ʼ��ADC���ƼĴ���1 ADC1_CR=0X8003 */
    EQADC.CFCR[0].B.SSE = 0x01; /* Trigger the CFIFO0 to config ACDxs��Set the SSSx bit */
   
    
    while (EQADC.CFSR.B.CFS0 == 0x3) /* Wait for trigger state to end so we konw queue is done��Current CFIFO Status=3��CFIFO is triggered*/
    {
    	;/* no deal with */
    }
    
    EQADC.FISR[0].B.EOQF = 0x01; /* Clear End Of Queue flag */   
    EQADC.CFCR[0].R = 0x0000; /* disable the queue in preparation for queue mode change */ 
    
    while (EQADC.CFSR.B.CFS0 != 0) /* wait for queue to go to idle before setting mode to user setting */  
    {
    	;/* no deal with */
    }
    
    /* CFIFO0���� */
    EQADC.IDCR[0].R = 0x0003; /* 0x0303 CFIFO����ֹDMA����RFIFO��ʹ��DMA����, CFFE0 = 1 Enable CFIFO Fill DMA or Interrupt request
                                 CFFS0 = 1 Generate DMA request to move data from the system memory to CFIFOx  
                                         0 Generate interrupt request to move data from the system memory to CFIFOx
                                 RFDE0=1 Enable RFIFO Drain DMA or Interrupt request
                                 RFDS0=1 Generate DMA request to move data from RFIFOx to the system memory
                                       0 Generate interrupt request to move data from RFIFOx to the system memory */  
    EQADC.CFCR[0].R = 0x0010; /* Trigger CFIFO 0 using Single Scan SW mode�� 0x0090Ϊ�����������ģʽ */ 
}

/************************************************************************
* @brief 
* 		adConvCmdSend	����ADת������
* @param[in]  ch:ģ����ͨ����
* @param[out] ��		  
* @return     ��
************************************************************************/
void adConvCmdSend (uint8_t ch) 
{
    EQADC.CFPR[0].R = 0x00100000 + (ch << 8); /* Conversion command: convert channel ch 
                                                 with ADC0, set EOQ, and send result to RFIFO 1 */ 
    EQADC.CFCR[0].R = 0x0410; /* Trigger CFIFO 0 using Single Scan SW mode */
}

/************************************************************************
* @brief 
* 		adResultRead	��ȡADת����ѹֵ
* @param[in]  ch:ģ����ͨ����
* @param[out] ��		  
* @return     ADת�����ѹֵ
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



