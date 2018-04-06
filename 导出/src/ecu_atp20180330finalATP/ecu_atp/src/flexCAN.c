/*------------------------------------------------------------------------
   版权 2017-2027 中国航空工业集团第六三一所第十八研究室
   
   对本文件的拷贝、发布、修改或者其他任何用途必须得到
   中国航空工业集团第六三一所的书面协议许可
   
   File : flexCAN.c
   
   Description : CAN总线驱动
   
   
   Revision History:
   rev.0.01 2017-11-21,马振华,创建
------------------------------------------------------------------------*/ 

#include "mpc5644a.h"  
#include "typedefs.h"  
#include "flexCAN.h"


/************************************************************************
* @brief 
* 		initCAN_A	初始化CANA模块
* @param[in]  br:波特率 1-100k 2-50k 3-25k 4-20k
*             ide:帧模式 0-标准帧 1-扩展帧
*			  id:id号
*			  wpe:Weak pullup/down enable
*			  wps:Weak pullup/down select
* @param[out] 无		  
* @return     无
************************************************************************/
void initCAN_A(uint8_t br, uint8_t ide, vuint32_t id) 
{
    uint8_t i = 0;
    
    /*FRZ = 1，HALT = 1进入冻结模式*/
    CAN_A.MCR.R = 0x5000003F; /* Put in Freeze Mode & enable all 64 msg buffers
                                 MDIS=0: Enable the FlexCAN module
                                 FRZ = 1: Enabled to enter Freeze Mode 
                                 FEN=0: FIFO not enabled                                          
                                 HALT = 1: Enters Freeze Mode if the FRZ bit is asserted 
                                 NOTRDY=0(read-only bit): FlexCAN module is either in Normal Mode, Listen-Only Mode or Loop-Back Mode
                                 WAK_MSK=0: Wake Up Interrupt is disabled
                                 SOFTRST=0: No reset request
                                 FRZACK=0(read-only bit): FlexCAN not in Freeze Mode, prescaler running
                                 SUPV=0: Affected registers are in Unrestricted memory space
                                 SLF_WAK=0: FlexCAN Self Wake Up feature is disabled      
                                 WRNEN=0: TWRNINT and RWRNINT bits are zero, independent of the values in the error counters
                                 MDISACK=0(read-only bit): FlexCAN not in any of the low power modes   
                                 WAK_SRC=0: FlexCAN uses the unfiltered Rx input to detect recessive to dominant edges on the CAN bus   
                                 SRX_DIS=0: 自接受使能,允许FlexCan模块接收由自己传送的帧，帧会存在MB内
                                 MBFEN=0: 使用全局掩码寄存器RXGMASK,RX14MASK and RX15MASK进行ID过滤. 
                                       1：使能私有掩码和队列，即使用RXIMR[0~63]进行ID过滤 
                                 LPRIO_EN=0: 禁止本地优先使能。 MB的本地优先级PRIO只有在LPRIO_EN为1时有效，且仅发送MB有效，被附加到ID来定义优先级
                                 AEN=0: Abort disabled
                                 IDAM=00：Format-A ，每个ID过滤表元素有一个全ID(标准ID和扩展ID)  
                                 MAXMB=63：Maximum Number of Message Buffers is64MB
                                 */ 
    /* 波特率 = 时钟频率/(PRESDIV + 1)/(PSEG1 + 1)/(PSEG2 + 1)/(PROPSEG + 1) */                             
    //CAN_A.CR.R = 0x04DB0006; 
                               /* Configure for 8MHz OSC, 100kHz bit time   8000000/5/(4+4+7)= 106667
                                PRESDIV = 4(0~255), 预分频Sclock frequency = CPI clock frequency / (PRESDIV + 1)  
                                RJW=3(1~4)，Resync Jump Width = RJW + 1=4. RJW通常小于等于PSEG2
                                PSEG1=3(1~8)，Phase Buffer Segment 1 = (PSEG1 + 1) x Time-Quanta.
                                PSEG2=3(1~8)，Phase Buffer Segment 2 = (PSEG2 + 1) x Time-Quanta
                                BOFFMSK=0: Bus Off interrupt disabled
                                ERRMSK=0: Error interrupt disabled   
                                CLKSRC=0: The CAN engine clock source is the oscillator clock，1-bus clock
                                TWRNMSK=0: Tx Warning Interrupt disabled
                                RWRNMSK=0: Rx Warning Interrupt disabled
                                LPB=0: Loop Back disabled自循环禁止
                                SMP=0: 1点采样，1-3点采样
                                BOFFREC=0: Automatic recovering from Bus Off state enabled, according to CAN Spec 2.0 part B
                                TSYN=0: Timer Sync feature disabled
                                LBUF=0: 仲裁过程根据最高优先权来选择激活的发送邮箱。
                                        若CR[LBUF]置位，则第一个（最小邮箱号）激活的发送邮箱获得仲裁优先权，且MCR[LPRIO_EN]将不产生作用。    
                                LOM=0: Listen Only Mode is deactivated
                                PROPSEG=6(1~8),Propagation Segment Time = (PROPSEG + 1) * Time-Quanta,Time-Quantum = one Sclock period.
                                */
    //CAN_A.CR.R = 0x18DB0006;        // Configure for 40MHz OSC, 100kHz bit time 
    CAN_A.CR.B.CLKSRC =  0;        //1b,0-oscillator clock;1-bus clock	
	switch (br)
	{
		case (0xff):		           //由于晶振频率的问题，需要调整晶振才能达到1M的波特率
		case (1):                 //number of time quanta units per bit rate period = 16
			CAN_A.CR.B.PRESDIV=  4;        //8b	8M/ 5/16=100K
			CAN_A.CR.B.PSEG1  =  3;        //3b,0-7:
			CAN_A.CR.B.PSEG2  =  3;        //3b,1-7:
			CAN_A.CR.B.RJW=3;
			CAN_A.CR.B.PROPSEG=  6;        //3b	= 16 -C Phase_Seg1 - Phase_Seg2 - SYNCSEG = 16 - 4 - 4 - 1 = 7; PROPSEG = 6	
		break;
		case (2):
			CAN_A.CR.B.PRESDIV=  9;        //8b	8M/10/16=50K
			CAN_A.CR.B.PSEG1  =  3;        //3b,0-7:
			CAN_A.CR.B.PSEG2  =  3;        //3b,1-7:
			CAN_A.CR.B.RJW=3;
			CAN_A.CR.B.PROPSEG=  6; 
		break;
		case (3):
			CAN_A.CR.B.PRESDIV= 19;        //8b	8M/20/16=25K
			CAN_A.CR.B.PSEG1  =  3;        //3b,0-7:
			CAN_A.CR.B.PSEG2  =  3;        //3b,1-7:
			CAN_A.CR.B.RJW=3;
			CAN_A.CR.B.PROPSEG=  6; 
		break;			
		case (4):
			CAN_A.CR.B.PRESDIV= 24;        //8b	8M/25/16=20K
			CAN_A.CR.B.PSEG1  =  3;        //3b,0-7:
			CAN_A.CR.B.PSEG2  =  3;        //3b,1-7:
			CAN_A.CR.B.RJW=3;
			CAN_A.CR.B.PROPSEG=  6; 
		break;
		default:
			CAN_A.CR.B.PRESDIV=  4;        //8b	8M/ 5/16=100K
			CAN_A.CR.B.PSEG1  =  3;        //3b,0-7:
			CAN_A.CR.B.PSEG2  =  3;        //3b,1-7:
			CAN_A.CR.B.RJW=3;
			CAN_A.CR.B.PROPSEG=  6; 
		break;
	}		                           
    for (i=0; i<64; i++) 
    {
        CAN_A.BUF[i].CS.B.CODE = 0; /* Inactivate all message buffers */
    }  
    
    CAN_A.BUF[0].CS.B.CODE = 8; /* Message Buffer 0 set to TX INACTIVE */
    
    /* 新增使用CAN_A.BUF[4]接收 */
    CAN_A.BUF[4].CS.B.IDE = ide;
    if(ide == 0) /* 标准帧 */
	{
		CAN_A.BUF[4].ID.B.STD_ID = id; 
	}
	else /* 扩展帧 */
	{
		CAN_A.BUF[4].ID.R = id;
	}
    CAN_A.BUF[4].CS.B.CODE = 4; /* MB 4 set to RX EMPTY */
    CAN_A.RXGMASK.R = 0x1FFFFFFF; /* 1-过滤ID相应比特位,0-ID相应比特位无关，由于ID最大29位，故高三位置0 */ 
    //CAN_A.RXGMASK.R = 0x00000000;   // MB4 设置为 接受所有ID的数据  
    
    SIU.PCR[83].R = 0x062C; /* MPC5644A: Configure pad as CNTXA, open drain.
                               pa=1,obe=1,Output buffer for the pad is enabled.
                               ode=1, Open drain is enabled for the pad.
                               src=3, Maximum slew rate*/
    SIU.PCR[84].R = 0x0500; /* MPC5644A: Configure pad as CNRXA pa=1,ibe=1,Input buffer for the pad is enabled.*/
    CAN_A.MCR.R = 0x0000003F; /* MDIS=0: Enable the FlexCAN module */
}

/************************************************************************
* @brief 
* 		initCAN_B	初始化CANB模块
* @param[in]  br:波特率 1-100k 2-50k 3-25k 4-20k
*             ide:帧模式 0-标准帧 1-扩展帧
*			  id:id号
*			  wpe:Weak pullup/down enable
*			  wps:Weak pullup/down select
* @param[out] 无		  
* @return     无
************************************************************************/
void initCAN_B(uint8_t br, uint8_t ide, vuint32_t id) 
{
    uint8_t i = 0;
    
    /*FRZ = 1，HALT = 1进入冻结模式*/
    CAN_B.MCR.R = 0x5000003F; /* Put in Freeze Mode & enable all 64 msg buffers
                                 MDIS=0: Enable the FlexCAN module
                                 FRZ = 1: Enabled to enter Freeze Mode 
                                 FEN=0: FIFO not enabled                                          
                                 HALT = 1: Enters Freeze Mode if the FRZ bit is asserted 
                                 NOTRDY=0(read-only bit): FlexCAN module is either in Normal Mode, Listen-Only Mode or Loop-Back Mode
                                 WAK_MSK=0: Wake Up Interrupt is disabled
                                 SOFTRST=0: No reset request
                                 FRZACK=0(read-only bit): FlexCAN not in Freeze Mode, prescaler running
                                 SUPV=0: Affected registers are in Unrestricted memory space
                                 SLF_WAK=0: FlexCAN Self Wake Up feature is disabled      
                                 WRNEN=0: TWRNINT and RWRNINT bits are zero, independent of the values in the error counters
                                 MDISACK=0(read-only bit): FlexCAN not in any of the low power modes   
                                 WAK_SRC=0: FlexCAN uses the unfiltered Rx input to detect recessive to dominant edges on the CAN bus   
                                 SRX_DIS=0: 自接受使能,允许FlexCan模块接收由自己传送的帧，帧会存在MB内
                                 MBFEN=0: 使用全局掩码寄存器RXGMASK,RX14MASK and RX15MASK进行ID过滤. 
                                       1：使能私有掩码和队列，即使用RXIMR[0~63]进行ID过滤 
                                 LPRIO_EN=0: 禁止本地优先使能。 MB的本地优先级PRIO只有在LPRIO_EN为1时有效，且仅发送MB有效，被附加到ID来定义优先级
                                 AEN=0: Abort disabled
                                 IDAM=00：Format-A ，每个ID过滤表元素有一个全ID(标准ID和扩展ID)  
                                 MAXMB=63：Maximum Number of Message Buffers is64MB
                                 */ 
    /* 波特率 = 时钟频率/(PRESDIV + 1)/(PSEG1 + 1)/(PSEG2 + 1)/(PROPSEG + 1) */                             
    //CAN_B.CR.R = 0x04DB0006; 
                               /* Configure for 8MHz OSC, 100kHz bit time   8000000/5/(4+4+7)= 106667
                                PRESDIV = 4(0~255), 预分频Sclock frequency = CPI clock frequency / (PRESDIV + 1)  
                                RJW=3(1~4)，Resync Jump Width = RJW + 1=4. RJW通常小于等于PSEG2
                                PSEG1=3(1~8)，Phase Buffer Segment 1 = (PSEG1 + 1) x Time-Quanta.
                                PSEG2=3(1~8)，Phase Buffer Segment 2 = (PSEG2 + 1) x Time-Quanta
                                BOFFMSK=0: Bus Off interrupt disabled
                                ERRMSK=0: Error interrupt disabled   
                                CLKSRC=0: The CAN engine clock source is the oscillator clock，1-bus clock
                                TWRNMSK=0: Tx Warning Interrupt disabled
                                RWRNMSK=0: Rx Warning Interrupt disabled
                                LPB=0: Loop Back disabled自循环禁止
                                SMP=0: 1点采样，1-3点采样
                                BOFFREC=0: Automatic recovering from Bus Off state enabled, according to CAN Spec 2.0 part B
                                TSYN=0: Timer Sync feature disabled
                                LBUF=0: 仲裁过程根据最高优先权来选择激活的发送邮箱。
                                        若CR[LBUF]置位，则第一个（最小邮箱号）激活的发送邮箱获得仲裁优先权，且MCR[LPRIO_EN]将不产生作用。    
                                LOM=0: Listen Only Mode is deactivated
                                PROPSEG=6(1~8),Propagation Segment Time = (PROPSEG + 1) * Time-Quanta,Time-Quantum = one Sclock period.
                                */
    //CAN_B.CR.R = 0x18DB0006;        // Configure for 40MHz OSC, 100kHz bit time 
    CAN_B.CR.B.CLKSRC =  0;        //1b,0-oscillator clock;1-bus clock	
	switch (br)
	{
		case (0xff):		           //由于晶振频率的问题，需要调整晶振才能达到1M的波特率
		case (1):                 //number of time quanta units per bit rate period = 16
			CAN_B.CR.B.PRESDIV=  4;        //8b	8M/ 5/16=100K
			CAN_B.CR.B.PSEG1  =  3;        //3b,0-7:
			CAN_B.CR.B.PSEG2  =  3;        //3b,1-7:
			CAN_B.CR.B.RJW=3;
			CAN_B.CR.B.PROPSEG=  6;        //3b	= 16 -C Phase_Seg1 - Phase_Seg2 - SYNCSEG = 16 - 4 - 4 - 1 = 7; PROPSEG = 6	
		break;
		case (2):
			CAN_B.CR.B.PRESDIV=  9;        //8b	8M/10/16=50K
			CAN_B.CR.B.PSEG1  =  3;        //3b,0-7:
			CAN_B.CR.B.PSEG2  =  3;        //3b,1-7:
			CAN_B.CR.B.RJW=3;
			CAN_B.CR.B.PROPSEG=  6; 
		break;
		case (3):
			CAN_B.CR.B.PRESDIV= 19;        //8b	8M/20/16=25K
			CAN_B.CR.B.PSEG1  =  3;        //3b,0-7:
			CAN_B.CR.B.PSEG2  =  3;        //3b,1-7:
			CAN_B.CR.B.RJW=3;
			CAN_B.CR.B.PROPSEG=  6; 
		break;			
		case (4):
			CAN_B.CR.B.PRESDIV= 24;        //8b	8M/25/16=20K
			CAN_B.CR.B.PSEG1  =  3;        //3b,0-7:
			CAN_B.CR.B.PSEG2  =  3;        //3b,1-7:
			CAN_B.CR.B.RJW=3;
			CAN_B.CR.B.PROPSEG=  6; 
		break;
		default:
			CAN_B.CR.B.PRESDIV=  4;        //8b	8M/ 5/16=100K
			CAN_B.CR.B.PSEG1  =  3;        //3b,0-7:
			CAN_B.CR.B.PSEG2  =  3;        //3b,1-7:
			CAN_B.CR.B.RJW=3;
			CAN_B.CR.B.PROPSEG=  6; 
		break;
	}		                           
    for (i=0; i<64; i++) 
    {
        CAN_B.BUF[i].CS.B.CODE = 0; /* Inactivate all message buffers */
    }  
    
    CAN_B.BUF[0].CS.B.CODE = 8; /* Message Buffer 0 set to TX INACTIVE */
    
    /* 使用CAN_B.BUF[4]接收 */
    CAN_B.BUF[4].CS.B.IDE = ide; 
    if(ide == 0) /* 标准帧 */
	{
		CAN_B.BUF[4].ID.B.STD_ID = id; 
	}
	else /* 扩展帧 */
	{
		CAN_B.BUF[4].ID.R = id;
	}
    CAN_B.BUF[4].CS.B.CODE = 4; /* MB 4 set to RX EMPTY */
    CAN_B.RXGMASK.R = 0x1FFFFFFF; /* 1-过滤ID相应比特位,0-ID相应比特位无关，由于ID最大29位，故高三位置0 */ 
    //CAN_B.RXGMASK.R = 0x00000000;   // MB4 设置为 接受所有ID的数据  
    
    SIU.PCR[85].R = 0x062C; /* MPC5644A: Configure pad as CNTXC, open drain */
    SIU.PCR[86].R = 0x0500; /* MPC5644A: Configure pad as CNRXC */
    CAN_B.MCR.R = 0x0000003F; /* Negate FlexCAN C halt state for 64 MB */
}

/************************************************************************
* @brief 
* 		CANATransmitMsg	CANA MB0发送信息
* @param[in]  ide:帧模式 0-标准帧 1-扩展帧
*			  id:id号
*			  txdata:发送缓冲区
*			  length:数据长度
* @param[out] 无		  
* @return     无
************************************************************************/
void CANATransmitMsg(uint8_t ide, vuint32_t id, uint8_t *txdata, uint8_t length) 
{
    uint8_t i = 0;    
    
    CAN_A.BUF[0].CS.B.IDE = ide;  /* 0-标准帧，1-扩展帧 */
	if(ide == 0) /* 标准帧 */
	{
		CAN_A.BUF[0].ID.B.STD_ID = id; 
	}
	else /* 扩展帧 */
	{
		CAN_A.BUF[0].ID.R = id;
	}
	
	CAN_A.BUF[0].CS.B.RTR = 0;
	
	CAN_A.BUF[0].CS.B.LENGTH = length;
	
	for (i=0; i<length && i<8; i++)
	{
		CAN_A.BUF[0].DATA.B[i] = txdata[i]; /* Data to be transmitted */
	}
	CAN_A.BUF[0].CS.B.SRR = 1; /* 替代远程请求位仅用于扩展帧，发送时该位必须置1*/
	CAN_A.BUF[0].CS.B.CODE = 0xC; /* Activate msg. buf. to transmit data frame */
}

/************************************************************************
* @brief 
* 		receiveMsg	CANB MB4接收信息
* @param[in]  无
* @param[out] 无		  
* @return     无
************************************************************************/
void receiveMsg (void) 
{
    uint8_t j = 0;
    uint32_t dummy = 0;
    uint32_t RxCODE = 0; /* Received message buffer code */
    uint32_t RxID = 0; /* Received message ID */
    uint32_t RxLENGTH = 0; /* Received message number of data bytes */
    uint8_t RxDATA[8] = {0}; /* Received message data string*/
    uint32_t RxTIMESTAMP = 0; /* Received message time */
    
    while (CAN_B.IFRL.B.BUF04I == 0) /* MPC5644A: Wait for CAN C MB 4 flag */
    {
        ;/* no deal with */	
    } 
     
    RxCODE = CAN_B.BUF[4].CS.B.CODE; /* Read CODE, ID, LENGTH, DATA, TIMESTAMP*/
    RxID = CAN_B.BUF[4].ID.B.STD_ID;
    RxLENGTH = CAN_B.BUF[4].CS.B.LENGTH;
    
    for (j=0; j<RxLENGTH; j++) 
    {
        RxDATA[j] = CAN_B.BUF[4].DATA.B[j];
    }
    RxTIMESTAMP = CAN_B.BUF[4].CS.B.TIMESTAMP;
    dummy = CAN_B.TIMER.R; /* Read TIMER to unlock message buffers */
    CAN_B.IFRL.R = 0x00000010; /* MPC5644A: Clear CAN C MB 4 flag */
}

/************************************************************************
* @brief 
* 		CANARecvMsg	CANA MB4接收信息
* @param[in]  *id:id号
*			  rxdata:接收缓冲区
*			  *length:数据长度
* @param[out] 无		  
* @return     无
************************************************************************/
void CANARecvMsg(vuint32_t *id, uint8_t *rxdata, uint8_t *length)
{   
	uint8_t j = 0;
  	uint32_t dummy = 0, code = 0, stamp = 0; 
  	
  	while (CAN_A.IFRL.B.BUF04I == 0) /* MPC5644A: Wait for CAN A MB 4 flag */
  	{
        ;/* no deal with */	
    }
    
	code   = CAN_A.BUF[4].CS.B.CODE;      /* Read CODE, ID, LENGTH, DATA, TIMESTAMP */
  	*id = CAN_A.BUF[4].ID.R;
  	*length = CAN_A.BUF[4].CS.B.LENGTH;
	
	for (j=0; j<*length; j++) 
	{ 
	    rxdata[j] = CAN_A.BUF[4].DATA.B[j];
	}
	stamp = CAN_A.BUF[4].CS.B.TIMESTAMP; 
  	dummy = CAN_A.TIMER.R;                /* Read TIMER to unlock message buffers */ 
	/* Clear CAN C MB n flag */
	CAN_A.IFRL.R = 0x00000010; /* MPC5644A: Clear CAN C MB 4 flag */
}

/************************************************************************
* @brief 
* 		CANBTransmitMsg	CANB MB0发送信息
* @param[in]  ide:帧模式 0-标准帧 1-扩展帧
*			  id:id号
*			  txdata:发送缓冲区
*			  length:数据长度
* @param[out] 无		  
* @return     无
************************************************************************/
void CANBTransmitMsg(uint8_t ide, vuint32_t id, uint8_t *txdata, uint8_t length) 
{
    uint8_t i = 0;    
    
    CAN_B.BUF[0].CS.B.IDE = ide;  /* 0-标准帧，1-扩展帧 */
	if(ide == 0) /* 标准帧 */
	{
		CAN_B.BUF[0].ID.B.STD_ID = id; 
	}
	else /* 扩展帧 */
	{
		CAN_B.BUF[0].ID.R = id;
	}
	
	CAN_B.BUF[0].CS.B.RTR = 0;
	
	CAN_B.BUF[0].CS.B.LENGTH = length;
	
	for (i=0; i<length && i<8; i++)
	{
		CAN_B.BUF[0].DATA.B[i] = txdata[i]; /* Data to be transmitted */
	}
	CAN_B.BUF[0].CS.B.SRR = 1; /* 替代远程请求位仅用于扩展帧，发送时该位必须置1*/
	CAN_B.BUF[0].CS.B.CODE = 0xC; /* Activate msg. buf. to transmit data frame */
}

void canTst(void)
{
    vuint32_t canaTstDat = 0x12345678, canbTstDat = 0x9ABCDEF0;
    
    CANATransmitMsg(1, 0x70, (uint8_t *)(&canaTstDat), 4);  
    delay(5); /* 延迟5msCANB发送数据 */
    CANBTransmitMsg(1, 0x71, (uint8_t *)(&canbTstDat), 4);
}