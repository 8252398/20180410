/* /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/   
��ƽ    ̨�������������ܿƼ�MPC�๦�ܿ�����
����    д��Chiu Sir
��E-mail  ��chiusir@163.com
������汾��V1.0
�������¡�2016��12��04��
�������Ϣ�ο����е�ַ��
����    վ��http://www.lqist.cn
���Ա����̡�http://shop36265907.taobao.com
------------------------------------------------
��dev.env.��Code Warrior 2.10
��Target  ��MPC5644A 0M14X
���ڲ����� khz
���ⲿ����40MHz-------
������Ƶ�ʡ�120MHz
/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/*/
#ifndef _LQ_ADC_
#define _LQ_ADC_

	#include "includes.h"

	/* EQADC_CAL = 1 means eQADC conversion with Calibration */
	/* EQADC_CAL = 0 means eQADC conversion uncalibrated */
	#define EQADC_CAL 1
	#define IDEAL_RES75 0x3000		// Ideal value of 75%(VDH-VDL)
	#define IDEAL_RES25 0x1000		// Ideal value of 25%(VDH-VDL)

extern	void EQADC_Init(void);
extern	int32_t EQADC_PowerUpCalibration(void);	
extern	uint32_t ConvertChannel(int32_t channel, int32_t bCal);
extern	int compare (const void * p0, const void * p1);
extern	uint32_t ConvertChannel_Median(int32_t channel, int32_t bCal);
	
extern	void initADC0(void);
extern	void SendConvCmd (void); 
extern uint32_t ReadResult(void); 

#endif