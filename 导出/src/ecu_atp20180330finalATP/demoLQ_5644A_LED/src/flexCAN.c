/*------------------------------------------------------------------------
   ��Ȩ 2017-2027 �й����չ�ҵ���ŵ�����һ����ʮ���о���
   
   �Ա��ļ��Ŀ������������޸Ļ��������κ���;����õ�
   �й����չ�ҵ���ŵ�����һ��������Э�����
   
   File : flexCAN.c
   
   Description : CAN��������
   
   
   Revision History:
   rev.0.01 2017-11-21,����,����
------------------------------------------------------------------------*/ 

#include "mpc5644a.h"  
#include "typedefs.h"  
#include "flexCAN.h"


/************************************************************************
* @brief 
* 		initCAN_A	��ʼ��CANAģ��
* @param[in]  br:������ 1-100k 2-50k 3-25k 4-20k
*             ide:֡ģʽ 0-��׼֡ 1-��չ֡
*			  id:id��
*			  wpe:Weak pullup/down enable
*			  wps:Weak pullup/down select
* @param[out] ��		  
* @return     ��
************************************************************************/
void initCAN_A(uint8_t br, uint8_t ide, vuint32_t id) 
{
    uint8_t i = 0;
    
    /*FRZ = 1��HALT = 1���붳��ģʽ*/
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
                                 SRX_DIS=0: �Խ���ʹ��,����FlexCanģ��������Լ����͵�֡��֡�����MB��
                                 MBFEN=0: ʹ��ȫ������Ĵ���RXGMASK,RX14MASK and RX15MASK����ID����. 
                                       1��ʹ��˽������Ͷ��У���ʹ��RXIMR[0~63]����ID���� 
                                 LPRIO_EN=0: ��ֹ��������ʹ�ܡ� MB�ı������ȼ�PRIOֻ����LPRIO_ENΪ1ʱ��Ч���ҽ�����MB��Ч�������ӵ�ID���������ȼ�
                                 AEN=0: Abort disabled
                                 IDAM=00��Format-A ��ÿ��ID���˱�Ԫ����һ��ȫID(��׼ID����չID)  
                                 MAXMB=63��Maximum Number of Message Buffers is64MB
                                 */ 
    /* ������ = ʱ��Ƶ��/(PRESDIV + 1)/(PSEG1 + 1)/(PSEG2 + 1)/(PROPSEG + 1) */                             
    //CAN_A.CR.R = 0x04DB0006; 
                               /* Configure for 8MHz OSC, 100kHz bit time   8000000/5/(4+4+7)= 106667
                                PRESDIV = 4(0~255), Ԥ��ƵSclock frequency = CPI clock frequency / (PRESDIV + 1)  
                                RJW=3(1~4)��Resync Jump Width = RJW + 1=4. RJWͨ��С�ڵ���PSEG2
                                PSEG1=3(1~8)��Phase Buffer Segment 1 = (PSEG1 + 1) x Time-Quanta.
                                PSEG2=3(1~8)��Phase Buffer Segment 2 = (PSEG2 + 1) x Time-Quanta
                                BOFFMSK=0: Bus Off interrupt disabled
                                ERRMSK=0: Error interrupt disabled   
                                CLKSRC=0: The CAN engine clock source is the oscillator clock��1-bus clock
                                TWRNMSK=0: Tx Warning Interrupt disabled
                                RWRNMSK=0: Rx Warning Interrupt disabled
                                LPB=0: Loop Back disabled��ѭ����ֹ
                                SMP=0: 1�������1-3�����
                                BOFFREC=0: Automatic recovering from Bus Off state enabled, according to CAN Spec 2.0 part B
                                TSYN=0: Timer Sync feature disabled
                                LBUF=0: �ٲù��̸����������Ȩ��ѡ�񼤻�ķ������䡣
                                        ��CR[LBUF]��λ�����һ������С����ţ�����ķ����������ٲ�����Ȩ����MCR[LPRIO_EN]�����������á�    
                                LOM=0: Listen Only Mode is deactivated
                                PROPSEG=6(1~8),Propagation Segment Time = (PROPSEG + 1) * Time-Quanta,Time-Quantum = one Sclock period.
                                */
    //CAN_A.CR.R = 0x18DB0006;        // Configure for 40MHz OSC, 100kHz bit time 
    CAN_A.CR.B.CLKSRC =  0;        //1b,0-oscillator clock;1-bus clock	
	switch (br)
	{
		case (0xff):		           //���ھ���Ƶ�ʵ����⣬��Ҫ����������ܴﵽ1M�Ĳ�����
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
    
    /* ����ʹ��CAN_A.BUF[4]���� */
    CAN_A.BUF[4].CS.B.IDE = ide;
    if(ide == 0) /* ��׼֡ */
	{
		CAN_A.BUF[4].ID.B.STD_ID = id; 
	}
	else /* ��չ֡ */
	{
		CAN_A.BUF[4].ID.R = id;
	}
    CAN_A.BUF[4].CS.B.CODE = 4; /* MB 4 set to RX EMPTY */
    CAN_A.RXGMASK.R = 0x1FFFFFFF; /* 1-����ID��Ӧ����λ,0-ID��Ӧ����λ�޹أ�����ID���29λ���ʸ���λ��0 */ 
    //CAN_A.RXGMASK.R = 0x00000000;   // MB4 ����Ϊ ��������ID������  
    
    SIU.PCR[83].R = 0x062C; /* MPC5644A: Configure pad as CNTXA, open drain.
                               pa=1,obe=1,Output buffer for the pad is enabled.
                               ode=1, Open drain is enabled for the pad.
                               src=3, Maximum slew rate*/
    SIU.PCR[84].R = 0x0500; /* MPC5644A: Configure pad as CNRXA pa=1,ibe=1,Input buffer for the pad is enabled.*/
    CAN_A.MCR.R = 0x0000003F; /* MDIS=0: Enable the FlexCAN module */
}

/************************************************************************
* @brief 
* 		initCAN_B	��ʼ��CANBģ��
* @param[in]  br:������ 1-100k 2-50k 3-25k 4-20k
*             ide:֡ģʽ 0-��׼֡ 1-��չ֡
*			  id:id��
*			  wpe:Weak pullup/down enable
*			  wps:Weak pullup/down select
* @param[out] ��		  
* @return     ��
************************************************************************/
void initCAN_B(uint8_t br, uint8_t ide, vuint32_t id) 
{
    uint8_t i = 0;
    
    /*FRZ = 1��HALT = 1���붳��ģʽ*/
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
                                 SRX_DIS=0: �Խ���ʹ��,����FlexCanģ��������Լ����͵�֡��֡�����MB��
                                 MBFEN=0: ʹ��ȫ������Ĵ���RXGMASK,RX14MASK and RX15MASK����ID����. 
                                       1��ʹ��˽������Ͷ��У���ʹ��RXIMR[0~63]����ID���� 
                                 LPRIO_EN=0: ��ֹ��������ʹ�ܡ� MB�ı������ȼ�PRIOֻ����LPRIO_ENΪ1ʱ��Ч���ҽ�����MB��Ч�������ӵ�ID���������ȼ�
                                 AEN=0: Abort disabled
                                 IDAM=00��Format-A ��ÿ��ID���˱�Ԫ����һ��ȫID(��׼ID����չID)  
                                 MAXMB=63��Maximum Number of Message Buffers is64MB
                                 */ 
    /* ������ = ʱ��Ƶ��/(PRESDIV + 1)/(PSEG1 + 1)/(PSEG2 + 1)/(PROPSEG + 1) */                             
    //CAN_B.CR.R = 0x04DB0006; 
                               /* Configure for 8MHz OSC, 100kHz bit time   8000000/5/(4+4+7)= 106667
                                PRESDIV = 4(0~255), Ԥ��ƵSclock frequency = CPI clock frequency / (PRESDIV + 1)  
                                RJW=3(1~4)��Resync Jump Width = RJW + 1=4. RJWͨ��С�ڵ���PSEG2
                                PSEG1=3(1~8)��Phase Buffer Segment 1 = (PSEG1 + 1) x Time-Quanta.
                                PSEG2=3(1~8)��Phase Buffer Segment 2 = (PSEG2 + 1) x Time-Quanta
                                BOFFMSK=0: Bus Off interrupt disabled
                                ERRMSK=0: Error interrupt disabled   
                                CLKSRC=0: The CAN engine clock source is the oscillator clock��1-bus clock
                                TWRNMSK=0: Tx Warning Interrupt disabled
                                RWRNMSK=0: Rx Warning Interrupt disabled
                                LPB=0: Loop Back disabled��ѭ����ֹ
                                SMP=0: 1�������1-3�����
                                BOFFREC=0: Automatic recovering from Bus Off state enabled, according to CAN Spec 2.0 part B
                                TSYN=0: Timer Sync feature disabled
                                LBUF=0: �ٲù��̸����������Ȩ��ѡ�񼤻�ķ������䡣
                                        ��CR[LBUF]��λ�����һ������С����ţ�����ķ����������ٲ�����Ȩ����MCR[LPRIO_EN]�����������á�    
                                LOM=0: Listen Only Mode is deactivated
                                PROPSEG=6(1~8),Propagation Segment Time = (PROPSEG + 1) * Time-Quanta,Time-Quantum = one Sclock period.
                                */
    //CAN_B.CR.R = 0x18DB0006;        // Configure for 40MHz OSC, 100kHz bit time 
    CAN_B.CR.B.CLKSRC =  0;        //1b,0-oscillator clock;1-bus clock	
	switch (br)
	{
		case (0xff):		           //���ھ���Ƶ�ʵ����⣬��Ҫ����������ܴﵽ1M�Ĳ�����
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
    
    /* ʹ��CAN_B.BUF[4]���� */
    CAN_B.BUF[4].CS.B.IDE = ide; 
    if(ide == 0) /* ��׼֡ */
	{
		CAN_B.BUF[4].ID.B.STD_ID = id; 
	}
	else /* ��չ֡ */
	{
		CAN_B.BUF[4].ID.R = id;
	}
    CAN_B.BUF[4].CS.B.CODE = 4; /* MB 4 set to RX EMPTY */
    CAN_B.RXGMASK.R = 0x1FFFFFFF; /* 1-����ID��Ӧ����λ,0-ID��Ӧ����λ�޹أ�����ID���29λ���ʸ���λ��0 */ 
    //CAN_B.RXGMASK.R = 0x00000000;   // MB4 ����Ϊ ��������ID������  
    
    SIU.PCR[85].R = 0x062C; /* MPC5644A: Configure pad as CNTXC, open drain */
    SIU.PCR[86].R = 0x0500; /* MPC5644A: Configure pad as CNRXC */
    CAN_B.MCR.R = 0x0000003F; /* Negate FlexCAN C halt state for 64 MB */
}

/************************************************************************
* @brief 
* 		CANATransmitMsg	CANA MB0������Ϣ
* @param[in]  ide:֡ģʽ 0-��׼֡ 1-��չ֡
*			  id:id��
*			  txdata:���ͻ�����
*			  length:���ݳ���
* @param[out] ��		  
* @return     ��
************************************************************************/
void CANATransmitMsg(uint8_t ide, vuint32_t id, uint8_t *txdata, uint8_t length) 
{
    uint8_t i = 0;    
    
    CAN_A.BUF[0].CS.B.IDE = ide;  /* 0-��׼֡��1-��չ֡ */
	if(ide == 0) /* ��׼֡ */
	{
		CAN_A.BUF[0].ID.B.STD_ID = id; 
	}
	else /* ��չ֡ */
	{
		CAN_A.BUF[0].ID.R = id;
	}
	
	CAN_A.BUF[0].CS.B.RTR = 0;
	
	CAN_A.BUF[0].CS.B.LENGTH = length;
	
	for (i=0; i<length && i<8; i++)
	{
		CAN_A.BUF[0].DATA.B[i] = txdata[i]; /* Data to be transmitted */
	}
	CAN_A.BUF[0].CS.B.SRR = 1; /* ���Զ������λ��������չ֡������ʱ��λ������1*/
	CAN_A.BUF[0].CS.B.CODE = 0xC; /* Activate msg. buf. to transmit data frame */
}

/************************************************************************
* @brief 
* 		receiveMsg	CANB MB4������Ϣ
* @param[in]  ��
* @param[out] ��		  
* @return     ��
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
* 		CANARecvMsg	CANA MB4������Ϣ
* @param[in]  *id:id��
*			  rxdata:���ջ�����
*			  *length:���ݳ���
* @param[out] ��		  
* @return     ��
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
* 		CANBTransmitMsg	CANB MB0������Ϣ
* @param[in]  ide:֡ģʽ 0-��׼֡ 1-��չ֡
*			  id:id��
*			  txdata:���ͻ�����
*			  length:���ݳ���
* @param[out] ��		  
* @return     ��
************************************************************************/
void CANBTransmitMsg(uint8_t ide, vuint32_t id, uint8_t *txdata, uint8_t length) 
{
    uint8_t i = 0;    
    
    CAN_B.BUF[0].CS.B.IDE = ide;  /* 0-��׼֡��1-��չ֡ */
	if(ide == 0) /* ��׼֡ */
	{
		CAN_B.BUF[0].ID.B.STD_ID = id; 
	}
	else /* ��չ֡ */
	{
		CAN_B.BUF[0].ID.R = id;
	}
	
	CAN_B.BUF[0].CS.B.RTR = 0;
	
	CAN_B.BUF[0].CS.B.LENGTH = length;
	
	for (i=0; i<length && i<8; i++)
	{
		CAN_B.BUF[0].DATA.B[i] = txdata[i]; /* Data to be transmitted */
	}
	CAN_B.BUF[0].CS.B.SRR = 1; /* ���Զ������λ��������չ֡������ʱ��λ������1*/
	CAN_B.BUF[0].CS.B.CODE = 0xC; /* Activate msg. buf. to transmit data frame */
}

void canTst(void)
{
    vuint32_t canaTstDat = 0x12345678, canbTstDat = 0x9ABCDEF0;
    
    CANATransmitMsg(1, 0x70, (uint8_t *)(&canaTstDat), 4);  
    delay(5); /* �ӳ�5msCANB�������� */
    CANBTransmitMsg(1, 0x71, (uint8_t *)(&canbTstDat), 4);
}