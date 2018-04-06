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
#include "LQ_UART.h"


void initESCIA (void) {
  ESCI_A.CR2.R = 0x2000;      /* Module is enabled (default setting ) */
  //BDR=sysclk/16/baudrate
  //ESCI_A.CR1.R = 0x01A1000C;  /* 64M:9600 baud, 8 bits, no parity, Tx & Rx enabled */
  //ESCI_A.CR1.R = 0x0208000C;  /* 80M:9600 baud, 8 bits, no parity, Tx & Rx enabled */
  //ESCI_A.CR1.R = 0x028b000C;  /* 100M:9600 baud, 8 bits, no parity, Tx & Rx enabled */
  ESCI_A.CR1.R = 0x030d000C;  /* 120M:9600 baud, 8 bits, no parity, Tx & Rx enabled */
  //ESCI_A.CR1.R = 0x03a9000C;  /* 144M:9600 baud, 8 bits, no parity, Tx & Rx enabled */
  SIU.PCR[89].R = 0x600;      /* Configure pad for primary func: TxDA */
  SIU.PCR[90].R = 0x500;      /* Configure pad for primary func: RxDA */
}
void initESCIB (void) {
  ESCI_B.CR2.R = 0x2000;      /* Module is enabled (default setting ) */
  //ESCI_B.CR1.R = 0x01A1000C;  /* 64M:9600 baud, 8 bits, no parity, Tx & Rx enabled */
  //ESCI_B.CR1.R = 0x0208000C;  /* 80M:9600 baud, 8 bits, no parity, Tx & Rx enabled */
  //ESCI_B.CR1.R = 0x028b000C;  /* 100M:9600 baud, 8 bits, no parity, Tx & Rx enabled */
  ESCI_B.CR1.R = 0x030d000C;  /* 120M:9600 baud, 8 bits, no parity, Tx & Rx enabled */
  //ESCI_B.CR1.R = 0x03a9000C;  /* 144M:9600 baud, 8 bits, no parity, Tx & Rx enabled */
  SIU.PCR[91].R = 0x400;      /* Configure pad for primary func: TxDA */
  SIU.PCR[92].R = 0x400;      /* Configure pad for primary func: RxDA */
}

void SCIA_putchar(char c)
{
    while (ESCI_A.SR.B.TDRE == 0) {}       /* Wait for transmit data reg empty = 1 */
    ESCI_A.SR.R = 0x80000000;              /* Clear TDRE flag */
	ESCI_A.DR.B.D = c;          /* Transmit 8 bits Data */
}

unsigned char SCIA_getchar()
{
  while (ESCI_A.SR.B.RDRF == 0) {}    /* Wait for receive data reg full = 1 */
  ESCI_A.SR.R = 0x20000000;           /* Clear RDRF flag */ 

  return ESCI_A.DR.B.D;            /* Read byte of Data*/	
}

void SCIA_putstr(char *str)
{
	while(*str!=0)
	{
		SCIA_putchar(*str++);
	}
}

void SCIB_putchar(char c)
{
    while (ESCI_B.SR.B.TDRE == 0) {}       /* Wait for transmit data reg empty = 1 */
    ESCI_B.SR.R = 0x80000000;              /* Clear TDRE flag */
	ESCI_B.DR.B.D = c;                     /* Transmit 8 bits Data */
}

unsigned char SCIB_getchar()
{
  while (ESCI_B.SR.B.RDRF == 0) {}    /* Wait for receive data reg full = 1 */
  ESCI_B.SR.R = 0x20000000;           /* Clear RDRF flag */
  return ESCI_B.DR.B.D;               /* Read byte of Data*/	
}

void SCIB_putstr(char *str)
{
	while(*str!=0)
	{
		SCIB_putchar(*str++);
	}
}


/*******************************************************************************
* Local function prototypes
*******************************************************************************/
void Init_eSCI_A(void);
void TransmitData(const char * pBuf, const uint32_t cnt);
int32_t ReceiveData(char * const pBuf);

/*******************************************************************************
Function Name : Init_eSCI_A
Engineer      : 
Date          : 2016-12-04
Parameters    : 
Modifies      : 
Returns       : 
Notes         : initializes MPC5554's eSCI_A module for UART mode
Issues        : expecting system clock is 150/132/120/12MHz
*******************************************************************************/
void Init_eSCI_A (void) 
{
	
	SIU.PCR[89].R = 0x400;  // Configure pad for primary func: TxDA
	SIU.PCR[90].R = 0x400;  // Configure pad for primary func: RxDA
	
	/* note: SBR = fsys/(16 * desired baudrate) < 8191 */

	ESCI_A.CR2.R = 0x2000;  // module is enabled (default setting)
	//CORE_CLOCK_120MHz
	ESCI_A.CR1.B.SBR = 391; // 19200 baud, 8 bits, no parity	

	ESCI_A.CR1.B.TE = 1;    // Tx enabled
	ESCI_A.CR1.B.RE = 1;    // Rx enabled	

}


/*******************************************************************************
Function Name : TransmitData
Engineer      : 
Date          : 2016-12-04
Parameters    : pBuf - input string. won't be modified by the function
              : cnt  - input number of characters to be transmitted
Modifies      : 
Returns       : 
Notes         : Tx data on eSCI_A. polled mode. 
Issues        :  
*******************************************************************************/
void TransmitData(const char * const pBuf, const uint32_t cnt) 
{
    uint8_t	j = 0;                                 /* Dummy variable */
    for (j=0; j< cnt; j++) 
    {  /* Loop for character string */
    
        /* Wait for transmit data reg empty = 1 */
        while (ESCI_A.SR.B.TDRE == 0) {}       
        ESCI_A.SR.R = 0x80000000;              /* Clear TDRE flag */
        ESCI_A.DR.B.D = *(pBuf+j);          /* Transmit 8 bits Data */        
    }
}

/*******************************************************************************
Function Name : ReceiveData
Engineer      : 
Date          : 2016-12-04
Parameters    : pBuf - address of a char where the received char is written to
                       the address (pBuf) doesn't change in the function
Modifies      : 
Returns       : 
Notes         : Rx data on eSCI_A. polled mode. 
Issues        :  
*******************************************************************************/
int32_t ReceiveData(char * const pBuf) 
{
    while (ESCI_A.SR.B.RDRF == 0) {}    /* Wait for receive data reg full = 1 */
    ESCI_A.SR.R = 0x20000000;           /* Clear RDRF flag */
    *pBuf = (char)ESCI_A.DR.B.D;               /* Read byte of Data*/  
    return 1;
}



/*******************************************************************************
* Global functions
*******************************************************************************/ 

/*
Methods called by MW MSL libraries to perform console IO:
*/

UARTError ReadUARTPoll(char* c) 
{
    if (ESCI_A.SR.B.RDRF == 0)
        return  kUARTNoData;  // return no data  
    else 
    {
        ESCI_A.SR.R = 0x20000000; // clear RDRF flag
        *c =(unsigned char) ESCI_A.DR.B.D; // read byte of Data
        return kUARTNoError;  // return no error
    }  
}


UARTError InitializeUART(UARTBaudRate baudRate)
{	
#pragma unused(baudRate)
	Init_eSCI_A();  
	return kUARTNoError; 
}


UARTError ReadUARTN(void * bytes, unsigned long length)
{ 
	unsigned long nchars = 0;
	while(  nchars < length)
	{
		int n = ReceiveData((char *)bytes+nchars);
		if(n<0) break;
		
		nchars += n;
	}; 
    return kUARTNoError; 
}


UARTError WriteUARTN(const void * bytes, unsigned long length)
{ 
	TransmitData ((const char * const)bytes,length);  
  	return kUARTNoError; 
}
