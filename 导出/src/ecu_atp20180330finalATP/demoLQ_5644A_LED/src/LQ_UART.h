/* /\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/   
��ƽ    ̨������MPC�๦�ܿ�����
����    д������
��Designed��by Chiu Sir
��E-mail  ��chiusir@aliyun.com
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
#ifndef _LQ_UART_
#define _LQ_UART_

	#include "MPC5644A.h"
	
	void initESCIA (void);
	void initESCIB (void);
	void SCIA_putchar(char c);
	unsigned char SCIA_getchar();
	void SCIA_putstr(char *str);
	void SCIB_putchar(char c);
	unsigned char SCIB_getchar();
	void SCIB_putstr(char *str);


	/****************************************************************************/

	typedef int UARTError;

	enum {
		kUARTNoError = 0,
		kUARTUnknownBaudRate,
		kUARTConfigurationError,
		kUARTBufferOverflow,				/* specified buffer was too small */
		kUARTNoData							/* no data available from polling */
	};

	/****************************************************************************/

	typedef enum {
		kBaudHWSet = -1,					/* use HW settings such as DIP switches */
		kBaud300 = 300,						/* valid baud rates */
		kBaud600 = 600,
		kBaud1200 = 1200,
		kBaud1800 = 1800,
		kBaud2000 = 2000,
		kBaud2400 = 2400,
		kBaud3600 = 3600,
		kBaud4800 = 4800,
		kBaud7200 = 7200,
		kBaud9600 = 9600,
		kBaud19200 = 19200,
		kBaud38400 = 38400,
		kBaud57600 = 57600,
		kBaud115200 = 115200,
		kBaud230400 = 230400
	} UARTBaudRate;

	/****************************************************************************/
    void Init_eSCI_A (void);
	UARTError InitializeUART(UARTBaudRate baudRate);
	UARTError TerminateUART(void);

	UARTError ReadUARTPoll(char* c);

	UARTError ReadUART1(char* c);
	UARTError ReadUARTN(void* bytes, unsigned long length);
	UARTError ReadUARTString(char* string, unsigned long limit, char terminateChar);

	UARTError WriteUART1(char c);
	UARTError WriteUARTN(const void* bytes, unsigned long length);
	UARTError WriteUARTString(const char* string);

#endif /* uart_h_included */

