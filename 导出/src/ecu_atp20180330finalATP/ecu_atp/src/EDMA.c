/*------------------------------------------------------------------------
   版权 2017-2027 中国航空工业集团第六三一所第十八研究室
   
   对本文件的拷贝、发布、修改或者其他任何用途必须得到
   中国航空工业集团第六三一所的书面协议许可
   
   File : EDMA.c
   
   Description : EDMA驱动
   
   
   Revision History:
   rev.0.01 2017-11-03,马振华,创建
------------------------------------------------------------------------*/
#include "mpc5644a.h"  
#include "typedefs.h"  

#if 0
const  uint8_t  SourceData[12] = {"Hello World\r"};	/* Source data string */
uint8_t  Destination[12] = {0};	                /* Destination byte */
    
/************************************************************************
* @brief 
* 		TCDnInit	EDMA通道TCD配置
* @param[in]  chan:通道号
* @param[out] 无		  
* @return     无
************************************************************************/
void TCDnInit(uint32_t chan) 
{
    EDMA.TCD[chan].SADDR = (vuint32_t) &SourceData;  /* Load address of source data */
    EDMA.TCD[chan].SSIZE = 0;                        /* Read 2**0 = 1 byte per transfer */
    EDMA.TCD[chan].SOFF = 1;                         /* After transfer, add 1 to src addr*/
    EDMA.TCD[chan].SLAST = -12;                      /* After major loop, reset src addr*/ 
    EDMA.TCD[chan].SMOD = 0;                         /* Source modulo feature not used */
    
    EDMA.TCD[chan].DADDR = (vuint32_t) &Destination; /* Load address of destination */
    EDMA.TCD[chan].DSIZE = 0;                        /* Write 2**0 = 1 byte per transfer */
    EDMA.TCD[chan].DOFF = 1;                         /* Do not increment destination addr */
    EDMA.TCD[chan].DLAST_SGA = -12;                    /* After major loop, no dest addr change*/ 
    EDMA.TCD[chan].DMOD = 0;                         /* Destination modulo feature not used */
    
    EDMA.TCD[chan].NBYTES = 1;                       /* Transfer 1 byte per minor loop */
    EDMA.TCD[chan].BITER = 12;                       /* 12 minor loop iterations */
    EDMA.TCD[chan].CITER = 12;                       /* Initialize current iteraction count */
    EDMA.TCD[chan].D_REQ = 0;                        /* Not disable channel when major loop is done*/
    EDMA.TCD[chan].INT_HALF = 0;                     /* Interrupts are not used */
    EDMA.TCD[chan].INT_MAJ = 0;                     /* 主循环完成中断禁止 */  
    EDMA.TCD[chan].CITERE_LINK = 0;                  /* Linking is not used */									  
    EDMA.TCD[chan].BITERE_LINK = 0;
    EDMA.TCD[chan].MAJORE_LINK = 0;                  /* Dynamic program is not used */ 
    EDMA.TCD[chan].MAJORLINKCH = 0x0;                /* Link Channel Number */
    EDMA.TCD[chan].E_SG = 0; 
    EDMA.TCD[chan].BWC = 0;                          /* Default bandwidth control- no stalls */
    EDMA.TCD[chan].START = 0;                        /* Initialize status flags */
    EDMA.TCD[chan].DONE = 0;
    EDMA.TCD[chan].ACTIVE = 0;
}


/************************************************************************
* @brief 
* 		EDMAInitTST	EDMA测试
* @param[in]  无
* @param[out] 无		  
* @return     无
************************************************************************/
void EDMAInitTST (void) 
{	
    uint32_t i = 0;              /* Dummy idle counter */ 
    uint32_t chan = 18,citer=0,start=0,active = 0, done = 0;
    uint8_t re = 1;
    
    TCDnInit(chan);           /* Initialize DMA Transfer Control Descriptor 0 */
    EDMA.CR.R = 0x0000E400; /* Use fixed priority arbitration for DMA groups and channels */ 
    EDMA.CPR[chan].R = chan;   /* 0x12 Channel 18 priorites: group priority = 1, channel priority = 2 */ 
    /* EDMA.EEIRL.B.EEI18 = 1; // The assertion of the error signal for channel 18 generate an error interrupt request 
       EDMA.EEIRH.R = 0x00000000;
       EDMA.EEIRL.R = 0x00040000;
    */
    EDMA.EEIRH.R = 0x00000000;
    EDMA.EEIRL.R = 0x00000000;
    EDMA.SERQR.R = chan;      /* Enable EDMA channel chan */  
    //EDMA.ERQRL.R = 1<<chan;
    /* EDMA_ERQRH.R = 0x00000000;
       EDMA_ERQRL.R = 0x00040000;*/
   
    /* Initate DMA service using software */  
    //EDMA.SSBR.R = chan;       /* Set channel 18 START bit to initiate first minor loop transfer */
    EDMA.TCD[chan].START = 1;                    
    
   
    while (1) 
    { 
    	i++; /* Loop forever */
    	
    	citer = EDMA.TCD[chan].CITER;
        start = EDMA.TCD[chan].START;
        active = EDMA.TCD[chan].ACTIVE;
        re = Destination[0];
        delay(10); //延迟10ms  
        EDMA.TCD[chan].START = 1;  /* 每10ms启动一次次要传输 */     
    }         
}
#endif

