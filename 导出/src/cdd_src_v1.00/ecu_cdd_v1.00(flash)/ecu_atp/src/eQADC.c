/*------------------------------------------------------------------------
   °æÈ¨ 2017-2027 ÖĞ¹úº½¿Õ¹¤Òµ¼¯ÍÅµÚÁùÈıÒ»ËùµÚÊ®°ËÑĞ¾¿ÊÒ
   
   ¶Ô±¾ÎÄ¼şµÄ¿½±´¡¢·¢²¼¡¢ĞŞ¸Ä»òÕßÆäËûÈÎºÎÓÃÍ¾±ØĞëµÃµ½
   ÖĞ¹úº½¿Õ¹¤Òµ¼¯ÍÅµÚÁùÈıÒ»ËùµÄÊéÃæĞ­ÒéĞí¿É
   
   File : eQADC.c
   
   Description : eQADCÇı¶¯
   
   
   Revision History:
   rev.0.01 2017-11-13,ÂíÕñ»ª,´´½¨
------------------------------------------------------------------------*/
#include "mpc5644a.h"  
#include "typedefs.h"  
#include "eQADC.h"

#define CFIFO_PUSH 0xFFF80010   /*PUSH 32Î»¿í*/
#define RFIFO_POP 0xFFF80032    /*POP È¡¼Ä´æÆ÷ÖĞµÍÊ®ÁùÎ»µÄÖµ*/   

vuint32_t RQUEUE[40] = {0};
vuint32_t CQUEUE0[40] = {0};


/************************************************************************
* @brief 
* 		eQadcChConfig	ÅäÖÃÄ£ÄâÁ¿ÊäÈë¹Ü½Å
* @param[in]  port:¶Ë¿Ú
*			  hys:Input hysteresis
*			  wpe:Weak pullup/down enable
*			  wps:Weak pullup/down select
* @param[out] ÎŞ		  
* @return     ÎŞ
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
* 		eQadcPcrConfig	ÅäÖÃÄ£ÄâÁ¿ÊäÈë¹Ü½ÅAN12~AN15
* @param[in]  ÎŞ
* @param[out] ÎŞ		  
* @return     ÎŞ
************************************************************************/
void eQadcPcrConfig(void)
{
	uint16_t hys = 0, wpe = 0, wps = 0;
	
	/* ÅçÓÍÊä³ö¹Ü½ÅÅäÖÃ */
	eQadcChConfig(215, hys, wpe, wps); /* SCR_OUTÎÂ¶È AN12 */
	eQadcChConfig(216, hys, wpe, wps); /* UREA_TANK_TEMP */
	eQadcChConfig(217, hys, wpe, wps); /* ¿Õ */
	eQadcChConfig(218, hys, wpe, wps); /* 24V¼à¿Ø */
}
 
/************************************************************************
* @brief 
* 		EDMAInitFcn	EDMAÅäÖÃ
* @param[in]  ÎŞ
* @param[out] ÎŞ		  
* @return     ÎŞ
************************************************************************/
void EDMAInitFcn(void) 
{	
	/* edma_global_config */
    EDMA.CR.R = 0x0000E400; /* Use fixed priority arbitration for DMA groups and channels */    
    EDMA.ERQRH.R = 0x00000000;
    EDMA.ERQRL.R = 0x00000003;  /* Ê¹Ä0~1Í¨µÀ */ 
    EDMA.EEIRH.R = 0x00000000;  /* ½ûÖ¹´íÎóÖĞ¶Ï */
    EDMA.EEIRL.R = 0x00000000;
    
    /* ÅäÖÃCFIFO 00 ¶ÔÓ¦µÄTCD edma_eqadc0_config */
    EDMA.TCD[0].SADDR = (vuint32_t)(&CQUEUE0[0]); /* Ô´µØÖ· */
	EDMA.TCD[0].DADDR = (vuint32_t)(&EQADC.CFPR[0].R); /*Ä¿±êµØÖ·(vuint32_t)&EQADC.CFPR[0].R*/
	EDMA.TCD[0].SMOD = 0x0; /* Ô´µØÖ·Ä£Êı£¬±íÊ¾×îµÍµÄnÎ»¿ÉÒÔ¸Ä±ä£¬´Ó¶øÌá¹©Ò»¸ö2^n×Ö½ÚµÄ»·ĞÎ¶ÓÁĞ */
	EDMA.TCD[0].DMOD = 0x0; /* Ä¿±êµØÖ·Ä£Êı */   
	EDMA.TCD[0].SSIZE = 0x2; /* Ä¿±ê´«Êä´óĞ¡32Î»£¬¼´Ã¿´Î´ÓÔ´µØÖ·¶Á32Î»Êı */
	EDMA.TCD[0].DSIZE = 0x2; /* Ô´´«Êä´óĞ¡£¬0-1B,1-2B,2-4B,3-8B,5-32B */   
	EDMA.TCD[0].SOFF = 0x0; /*ÓĞ·ûºÅÔ´Æ«ÒÆÁ¿*/
	EDMA.TCD[0].NBYTES = 0x4; /*Ò»´ÎÄÚ²ã´ÎÒªÑ­»·´«ÊäµÄ×Ö½ÚÊı,¼´Ã¿´ÎDMA´«ÊäµÄ×Ö½ÚÊı*/
	EDMA.TCD[0].SLAST = 0; /* -160 0xFFFFFF60Ä©¼¶Ä¿±êµØÖ·µ÷ÕûÖµ£¬Ö÷Ñ­»·½áÊøÊ±¼Óµ½Ô´µØÖ·ÉÏ*/   
	EDMA.TCD[0].DOFF = 0x0; /*ÓĞ·ûºÅÄ¿±êµØÖ·Æ«ÒÆÁ¿*/
	EDMA.TCD[0].DLAST_SGA = 0; /*Ä©¼¶ÓĞ·ûºÅÄ¿±êµØÖ·Æ«ÒÆÁ¿*/
	EDMA.TCD[0].BITER = 1; /*Ö÷Ñ­»·µü´ú¼ÆÊı³õÊ¼Öµ40*/
	EDMA.TCD[0].CITER = 1; /*Ö÷Ñ­»·µü´ú¼ÆÊıµ±Ç°Öµ*/   
	EDMA.TCD[0].BWC = 0x0; /*´ø¿í¿ØÖÆ£º0-ÎŞDMA´«ÊäÔİÍ££¬2-Îª¼õÉÙ×ÜÏßÉÏÆäËûÉè±¸·ÃÎÊÏàÍ¬×ÊÔ´µÄÊ±¼äÑÓ³Ù£¬
	                         eDMAÍê³ÉÃ¿´Î¶Á/Ğ´·ÃÎÊºóÔİÍ£4¸öÊ±ÖÓÖÜÆÚ £¬*/
	EDMA.TCD[0].MAJORLINKCH = 0x0; /*Ö÷Á´½ÓÍ¨µÀºÅ*/
	EDMA.TCD[0].MAJORE_LINK = 0x0; /*Ö÷Ñ­»·Íê³ÉÊ±£¬Í¨µÀÁ´½Ó½ûÖ¹*/
	EDMA.TCD[0].DONE = 0x00; /* Í¨µÀ·şÎñÍê³É£¬EDMAÔÚÖ÷Ñ­»·Íê³ÉÊ±ÉèÖÃ¸ÃÎ» */
	EDMA.TCD[0].ACTIVE = 0x00;  /* Í¨µÀ»îÔ¾£¬edmaÔÚÃ¿´Î´ÎÒªÑ­»·¿ªÊ¼Ê±ÖÃÎ»£¬½áÊøÊ±ÇåÁã */
	EDMA.TCD[0].E_SG = 0x0; /* ·ÖÉ¢/¾ÛºÏ²Ù×÷½ûÖ¹ */
	EDMA.TCD[0].D_REQ = 0x0; /* Ö÷Ñ­»·Íê³ÉÊ±½ûÖ¹Í¨µÀÇëÇó¡£ÔÚÊ¹ÓÃÓ²¼şÆô¶¯edmaÇëÇóÊ±£¬Èç¹û¸Ã¿ØÖÆÎ»±»ÉèÖÃ£¬
	                            ÔòÔÚÖ÷Ñ­»·½áÊøÊ±£¬»á½ûÖ¹DMAÇëÇó */
	EDMA.TCD[0].INT_HALF = 0x0; /* Ö÷Ñ­»·Ö´ĞĞÒ»°ëÊ±²»ÖĞ¶ÏÄÚºË */
	EDMA.TCD[0].INT_MAJ = 0x1; /* 1-Ö÷Ñ­»·Íê³ÉÖĞ¶ÏÄÚºË */
	EDMA.TCD[0].START = 0x0; /* Í¨µÀstratÎ»£¬ÔÚÏëÍ¨¹ıÈí¼ş·¢ÆğDMA·şÎñÇëÇóÊ±£¬¿ÉÒÔÍ¨¹ıÈí¼şÏò¸ÃÎ»Ğ´1Ã÷È·Æô¶¯¸ÃedmaÍ¨µÀ¡£
	                            Ò»µ©DMAÍ¨µÀÊ±Òò¸ÃÎ»ÉèÖÃ¿ªÊ¼Ö´ĞĞ£¬¸ÃÍ¨µÀÔÚÍê³ÉËùÓĞÊı¾İ´«ÊäÇ°²»ÄÜ±»Í£Ö¹ */   
	                            
	/* ÅäÖÃRFIFO 00 ¶ÔÓ¦µÄTCD */
    EDMA.TCD[1].SADDR = (vuint32_t)(&EQADC.RFPR[0].R); /* Ô´µØÖ· */
	EDMA.TCD[1].DADDR = (vuint32_t)(&RQUEUE[0]); /*Ä¿±êµØÖ·(vuint32_t)&EQADC.CFPR[0].R*/
	EDMA.TCD[1].SMOD = 0x0; /* Ô´µØÖ·Ä£Êı£¬±íÊ¾×îµÍµÄnÎ»¿ÉÒÔ¸Ä±ä£¬´Ó¶øÌá¹©Ò»¸ö2^n×Ö½ÚµÄ»·ĞÎ¶ÓÁĞ */
	EDMA.TCD[1].DMOD = 0x0; /* Ä¿±êµØÖ·Ä£Êı */   
	EDMA.TCD[1].SSIZE = 0x2; /* Ä¿±ê´«Êä´óĞ¡16Î»£¬¼´Ã¿´Î´ÓÔ´µØÖ·¶Á16Î»Êı */
	EDMA.TCD[1].DSIZE = 0x2; /* Ô´´«Êä´óĞ¡£¬0-1B,1-2B,2-4B,3-8B,5-32B */   
	EDMA.TCD[1].SOFF = 0x0; /*ÓĞ·ûºÅÔ´Æ«ÒÆÁ¿,Ã¿´ÎÔ´¶ÁÈ¡²Ù×÷Íê³Éºó¼Óµ½SADDRÉÏ*/
	EDMA.TCD[1].NBYTES = 0x4; /*Ò»´ÎÄÚ²ã´ÎÒªÑ­»·´«ÊäµÄ×Ö½ÚÊı,¼´Ã¿´ÎDMA´«ÊäµÄ×Ö½ÚÊı*/
	EDMA.TCD[1].SLAST = 0; /*Ä©¼¶Ä¿±êµØÖ·µ÷ÕûÖµ£¬Ö÷Ñ­»·½áÊøÊ±¼Óµ½Ô´µØÖ·ÉÏ*/   
	EDMA.TCD[1].DOFF = 0x0; /*ÓĞ·ûºÅÄ¿±êµØÖ·Æ«ÒÆÁ¿*/
	EDMA.TCD[1].DLAST_SGA = 0; /*Ä©¼¶ÓĞ·ûºÅÄ¿±êµØÖ·Æ«ÒÆÁ¿-80*/
	EDMA.TCD[1].BITER = 1; /*Ö÷Ñ­»·µü´ú¼ÆÊı³õÊ¼Öµ*/;
	EDMA.TCD[1].CITER = 1; /*Ö÷Ñ­»·µü´ú¼ÆÊıµ±Ç°Öµ*/;    
	EDMA.TCD[1].BWC = 0x0; /*´ø¿í¿ØÖÆ£ºÎŞDMA´«ÊäÔİÍ£*/
	EDMA.TCD[1].MAJORLINKCH = 0x0; /*Ö÷Á´½ÓÍ¨µÀºÅ*/
	EDMA.TCD[1].MAJORE_LINK = 0x0; /*Ö÷Ñ­»·Íê³ÉÊ±£¬Í¨µÀÁ´½Ó½ûÖ¹*/
	EDMA.TCD[1].DONE = 0x00; /* Í¨µÀ·şÎñÍê³É£¬EDMAÔÚÖ÷Ñ­»·Íê³ÉÊ±ÉèÖÃ¸ÃÎ» */
	EDMA.TCD[1].ACTIVE = 0x00;  /* Í¨µÀ»îÔ¾£¬edmaÔÚÃ¿´Î´ÎÒªÑ­»·¿ªÊ¼Ê±ÖÃÎ»£¬½áÊøÊ±ÇåÁã */
	EDMA.TCD[1].E_SG = 0x0; /* ·ÖÉ¢/¾ÛºÏ²Ù×÷½ûÖ¹ */
	EDMA.TCD[1].D_REQ = 0x0; /* Ö÷Ñ­»·Íê³ÉÊ±½ûÖ¹Í¨µÀÇëÇó¡£ÔÚÊ¹ÓÃÓ²¼şÆô¶¯edmaÇëÇóÊ±£¬Èç¹û¸Ã¿ØÖÆÎ»±»ÉèÖÃ£¬
	                            ÔòÔÚÖ÷Ñ­»·½áÊøÊ±£¬»á½ûÖ¹DMAÇëÇó */
	EDMA.TCD[1].INT_HALF = 0x0; /* Ö÷Ñ­»·Ö´ĞĞÒ»°ëÊ±²»ÖĞ¶ÏÄÚºË */
	EDMA.TCD[1].INT_MAJ = 0x0; /* Ö÷Ñ­»·Íê³ÉÖĞ¶Ï½ûÖ¹ */
	EDMA.TCD[1].START = 0x0; /* Í¨µÀstratÎ»£¬ÔÚÏëÍ¨¹ıÈí¼ş·¢ÆğDMA·şÎñÇëÇóÊ±£¬¿ÉÒÔÍ¨¹ıÈí¼şÏò¸ÃÎ»Ğ´1Ã÷È·Æô¶¯¸ÃedmaÍ¨µÀ¡£
	                            Ò»µ©DMAÍ¨µÀÊ±Òò¸ÃÎ»ÉèÖÃ¿ªÊ¼Ö´ĞĞ£¬¸ÃÍ¨µÀÔÚÍê³ÉËùÓĞÊı¾İ´«ÊäÇ°²»ÄÜ±»Í£Ö¹ */          
}

/************************************************************************
* @brief 
* 		eQADCInitFcn	eQADCÅäÖÃ
* @param[in]  ÎŞ
* @param[out] ÎŞ		  
* @return     ÎŞ
************************************************************************/
void eQADCInitFcn(void) 
{
    /* ×ª»»ÃüÁîÅäÖÃ */	
    CQUEUE0[0] = 0x80001000;  /*  16Í¨µÀ¹ìÑ¹²É¼¯Ê¹ÄÜÊ±¼ä´Á 
                                  bit1£ºEOQ=1,µ¥´ÎÉ¨ÃèÄ£Ê½ÇÒ´¦ÓÚÃüÁî¶ÓÁĞÄ©Î²ÖÃ1£¨1 Last entry of the CQueue£© 
	                              bit8~11:MESSAGE_TAG[0:3]=0,Result is sent to RFIFO 0 (0~5¶ÔÓ¦RFIFO0~5)
	                              bit12~13:lst[0:1]=0,Sampling cycles(ADC Clock Cycles)=2 (1-8,2-64,3-128)
	                              bit14:TSR=1,Í¨µÀÊ¹ÄÜÊ±¼ä´Á(Return conversion time stamp after the conversion result.) */
	
 
    /* eqadc_global_config */
    EQADC.MCR.R = 0x00000000;		/* ICEA0=0 Disable immediate conversion command request
                                       ICEA1=0 Disable immediate conversion command request  
                                       ESSIE = 0 EQADC SSI is disabled
                                       DBG=0 Do not enter debug mode */
	EQADC.NMSFR.R = 0x00000000;	/* Null Message Format:The value written to this register will be sent as 
	                               a null message when serial transmissions from the EQADC SSI are enabled*/
	EQADC.ETDFR.R = 0x00000000;	/* DFL(bit28-31):Digital Filter Length = 2  */

	
	/* CFIFO0ÅäÖÃ */
	EQADC.CFCR[0].R = 0x0010; /* Trigger CFIFO 0 using Single Scan SW mode 
	                             CFEEE0= 0 CFIFO0 has a normal value of entries  
	                             STRME0=0 Streaming mode of CFIFO0 is disabled
	                             SSE0=0 No effect.
	                             CFINV0=0 No effect.
	                             MODE0=1 Software Trigger, Single Scan
	                             AMODE0=0 Disable CFIFO0 Advance Trigger Operation Mode 0 */ 
	EQADC.IDCR[0].R = 0x0000; /* Disable all interrupt and DMA request. */
	
	/* eqadc_adc0_config */
    EQADC.CFPR[0].B.CFPUSH = 0x00000802; /* ³õÊ¼»¯Ê±¼ä´Á¿ØÖÆ¼Ä´æÆ÷0 µÈ¼ÛÓÚEQADC.CFPR[0].R£¬ADC_TSCR=0X0008,Clock Divide Factor for Time Stamp 16·ÖÆµ*/
    EQADC.CFPR[0].B.CFPUSH = 0x00000003; /* ³õÊ¼»¯Ê±»ù¼ÆÊı¼Ä´æÆ÷0 ADC_TBCR=0X0000,ÉèÖÃÊ±»ù¼ÆÊıÆ÷Îª0*/
    EQADC.CFPR[0].B.CFPUSH = 0x00800301; /* ³õÊ¼»¯ADC¿ØÖÆ¼Ä´æÆ÷0 ADC0_CR=0X8003,bit12~15=0x03,Clock Divide Factor for Time Stamp 8·ÖÆµ  (0x9-20·ÖÆµ£¬¼´×î´óËÙ¶È12MHz) 
                                                bit4=0 External multiplexer disabled; no external multiplexer channels can be selected.
                                                bit0=1 1 ADC is enabled and ready to perform A/D conversions. */
    EQADC.CFPR[0].B.CFPUSH = 0x82800301; /* ³õÊ¼»¯ADC¿ØÖÆ¼Ä´æÆ÷1 ADC1_CR=0X8003 */
    EQADC.CFCR[0].B.SSE = 0x01; /* Trigger the CFIFO0 to config ACDxs£¬Set the SSSx bit */
   
    
    while (EQADC.CFSR.B.CFS0 == 0x3) /* Wait for trigger state to end so we konw queue is done£¬Current CFIFO Status=3£¬CFIFO is triggered*/
    {
    	;/* no deal with */
    }
    
    EQADC.FISR[0].B.EOQF = 0x01; /* Clear End Of Queue flag */   
    EQADC.CFCR[0].R = 0x0000; /* disable the queue in preparation for queue mode change */ 
    
    while (EQADC.CFSR.B.CFS0 != 0) /* wait for queue to go to idle before setting mode to user setting */  
    {
    	;/* no deal with */
    }
    
    /* CFIFO0ÅäÖÃ */
    EQADC.IDCR[0].R = 0x0003; /* 0x0303 CFIFOÂú½ûÖ¹DMAÇëÇóRFIFO¿ÕÊ¹ÄÜDMAÇëÇó, CFFE0 = 1 Enable CFIFO Fill DMA or Interrupt request
                                 CFFS0 = 1 Generate DMA request to move data from the system memory to CFIFOx  
                                         0 Generate interrupt request to move data from the system memory to CFIFOx
                                 RFDE0=1 Enable RFIFO Drain DMA or Interrupt request
                                 RFDS0=1 Generate DMA request to move data from RFIFOx to the system memory
                                       0 Generate interrupt request to move data from RFIFOx to the system memory */  
    EQADC.CFCR[0].R = 0x0010; /* Trigger CFIFO 0 using Single Scan SW mode£¬ 0x0090ÎªÈí¼şÁ¬Ğø´¥·¢Ä£Ê½ */ 
}

/************************************************************************
* @brief 
* 		adConvCmdSend	·¢ËÍAD×ª»»ÃüÁî
* @param[in]  ch:Ä£ÄâÁ¿Í¨µÀºÅ
* @param[out] ÎŞ		  
* @return     ÎŞ
************************************************************************/
void adConvCmdSend (uint8_t ch) 
{
    EQADC.CFPR[0].R = 0x00100000 + (ch << 8); /* Conversion command: convert channel ch 
                                                 with ADC0, set EOQ, and send result to RFIFO 1 */ 
    EQADC.CFCR[0].R = 0x0410; /* Trigger CFIFO 0 using Single Scan SW mode */
}

/************************************************************************
* @brief 
* 		adResultRead	¶ÁÈ¡AD×ª»»µçÑ¹Öµ
* @param[in]  ch:Ä£ÄâÁ¿Í¨µÀºÅ
* @param[out] ÎŞ		  
* @return     AD×ª»»ºóµçÑ¹Öµ
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



