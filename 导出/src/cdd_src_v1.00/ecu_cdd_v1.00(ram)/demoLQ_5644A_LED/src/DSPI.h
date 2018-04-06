/*------------------------------------------------------------------------
   ��Ȩ 2017-2027 �й����չ�ҵ���ŵ�����һ����ʮ���о���
   
   �Ա��ļ��Ŀ������������޸Ļ��������κ���;����õ�
   �й����չ�ҵ���ŵ�����һ��������Э�����
   
   File : DSPI.h
   
   Description : PLL������ͷ�ļ�
   
   Revision History :
   rev.0.01 2018-01-02,����,����
------------------------------------------------------------------------*/
#ifndef DSPI_H_
#define DSPI_H_

#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
   include files
------------------------------------------------------------------------*/


/*------------------------------------------------------------------------
   macro definition
------------------------------------------------------------------------*/
#define CTLCODE 0x80040000
#define CTLENDCODE 0x00040000
#define CTLENDEOQCODE 0x08040000

/*------------------------------------------------------------------------
   extern variables declaration
------------------------------------------------------------------------*/


/*------------------------------------------------------------------------
   extern function declaration
------------------------------------------------------------------------*/
extern void dspiBInit(void);
extern int32_t dspiDataSend(uint32_t *dataBuf, uint32_t len);
extern int32_t dspiBDataRecv(uint32_t *dataBuf);
extern void delay(int ms);
extern void dspiTst(void); 
extern void dspi_init (void);
extern void delayCnt (vuint32_t tCount);
extern void dspiPeriodTask(void);
extern void sampleKeySwitch(void);

#ifdef __cplusplus
}
#endif

#endif		/* end DSPI.h*/
